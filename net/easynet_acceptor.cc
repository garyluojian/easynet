#include "easynet_eventloop.h"
#include "easynet_acceptor.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        Acceptor::Acceptor(
            EventLoop *loop,
            const std::string &ip, unsigned short port)
            : _loop(loop),
            _acceptor_ip(ip),
            _acceptor_port(port),
            _acceptor_socket(Socket::SOCK_TYPE_TCP),
            _channel(_loop, _acceptor_socket.fd())
        {
        }

        int Acceptor::init()
        {
            _acceptor_socket.setLinger(0, 0);
            _acceptor_socket.setAddrReuse();
            int rc = _acceptor_socket.bind(_acceptor_ip, _acceptor_port);
            if(rc)
            {
                LOG(ERROR)<<"Bind to "<<_acceptor_ip<<":"<<_acceptor_port<<"failed.";
                return -1;
            }
            _channel.setReadCallback(std::bind(&Acceptor::handleRead, this));
            _channel.enableRead();
            rc = _acceptor_socket.listen(5);
            if(rc)
            {
                LOG(ERROR)<<"Listen on "<<_acceptor_ip<<":"<<_acceptor_port<<"failed.";
                return -1;
            }
            LOG(INFO)<<"Acceptor("<<_acceptor_ip<<":"<<_acceptor_port<<") init OK.";

            return 0;
        }

        void Acceptor::stop()
        {
            _channel.disableAll();
            _acceptor_socket.close();
        }

        void Acceptor::handleRead()
        {
            std::string accepted_ip;
            unsigned short accepted_port;

            int fd = _acceptor_socket.accept(accepted_ip, accepted_port);

            if(fd < 0)
            {
                LOG(ERROR)<<"Aceept failed.";
                return;
            }
            std::shared_ptr<TcpConnection> new_connection 
                = std::make_shared<TcpConnection>(_loop, fd);
            new_connection->setPeerAddr(accepted_ip, accepted_port);
            new_connection->setMessageCallback(_message_callback);
            new_connection->setCloseCallback(_on_close_callback);
            new_connection->enableRead();

            _new_connection_callback(new_connection);
        }
    }
}
