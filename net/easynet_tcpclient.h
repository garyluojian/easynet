#include "easynet_eventloop.h"
#include "easynet_channel.h"
#include "easynet_socket.h"
#include "easynet_tcpconnection.h"
#include <string>
#include <memory>
#include <functional>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        class TcpClient
        {
        public:
            TcpClient(EventLoop *loop, const std::string &server_ip, unsigned short server_port);
            ~TcpClient();

            void init();
            void stop();
            void connect()
            {
                _connection->connect(_server_ip, _server_port);
            }

            void disconnect()
            {
                _connection->disconnect();
            }

            void send(const void *data, size_t len)
            {
                _connection->send(data, len);
            }

            void setMessageCallback(
                    const std::function<void(std::shared_ptr<TcpConnection>&, const void *data, size_t len)> &message_callback)
            {
                _message_callback = message_callback;
            }
            void setConnectionCallback(
                    const std::function<void(std::shared_ptr<TcpConnection>&)> &connection_callback)
            {
                _connect_done_callback = connection_callback;
            }
            void setDisconnectCallback(
                    const std::function<void(std::shared_ptr<TcpConnection>&)> &disconnection_callback)
            {
                _disconnect_callback = disconnection_callback;
            }

            void setAutoReconn(bool auto_reconn, int interval, int times)
            {
                _connection->setAutoReconn(auto_reconn, interval, times);
            }

            std::shared_ptr<TcpConnection> getConnection()
            {
                return _connection;
            }

        private:
            void onMessage(std::shared_ptr<TcpConnection> conn, const void *data, size_t len)
            {
                _message_callback(conn, data, len);
            }
            void onConnect(std::shared_ptr<TcpConnection> conn)
            {
                _connect_done_callback(conn);
            }
            void onDisConnect(std::shared_ptr<TcpConnection> conn)
            {
                _disconnect_callback(conn);
            }

            void defaultMessageCallback(std::shared_ptr<TcpConnection> conn, const void *data, size_t len)
            {
                (void)data;
                (void)len;
                LOG(INFO)<<"Recv some data from "<<conn->getConnectionName();
            }
            void defalutConnectCallback(std::shared_ptr<TcpConnection> conn){(void)conn;}
            void defalutDisconnectCallback(std::shared_ptr<TcpConnection> conn){(void)conn;}

        private:
            EventLoop *_loop;
            std::string _server_ip;
            unsigned short _server_port;
            std::shared_ptr<TcpConnection> _connection;
            std::function<void(std::shared_ptr<TcpConnection>&, const void *data, size_t len)> _message_callback;
            std::function<void(std::shared_ptr<TcpConnection>&)> _connect_done_callback;
            std::function<void(std::shared_ptr<TcpConnection>&)> _disconnect_callback;
        };
    }
}