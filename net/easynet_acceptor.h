#ifndef __EASYNET_ACCEPTOR_H__
#define __EASYNET_ACCEPTOR_H__

#include "easynet_channel.h"
#include "easynet_tcpconnection.h"
#include "easynet_socket.h"
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace easynet
{
    namespace net
    {
        class EventLoop;
        class Acceptor
        {
            public:
                Acceptor(EventLoop *loop, 
                    const std::string &ip, unsigned short port);
                ~Acceptor(){}
            public:
                int init();
                void stop();

                void setReadCallback(
                    const std::function<void(std::shared_ptr<TcpConnection>, const void*, size_t)> &on_message)
                {
                    _message_callback = on_message;
                }

                void setNewConnectionCallback(const std::function<void(std::shared_ptr<TcpConnection>)> &on_connect)
                {
                    _new_connection_callback = on_connect;
                }

                void setCloseConnectionCallback(const std::function<void(std::shared_ptr<TcpConnection>)> &close_callback)
                {
                    _on_close_callback = close_callback;
                }

            private:
                void handleRead();
            private:
                EventLoop *_loop;
                std::string _acceptor_ip;
                unsigned short _acceptor_port;
                Socket _acceptor_socket;
                Channel _channel;
                std::function<void(std::shared_ptr<TcpConnection>,const void*,size_t)> _message_callback;
                std::function<void(std::shared_ptr<TcpConnection>)> _new_connection_callback;
                std::function<void(std::shared_ptr<TcpConnection>)> _on_close_callback;
        };
    }
}

#endif
