#include "easynet_tcpclient.h"

namespace easynet
{
    namespace net
    {
        TcpClient::TcpClient(easynet::net::EventLoop *loop, const std::string &server_ip, unsigned short server_port)
            : _loop(loop), _server_ip(server_ip), _server_port(server_port),
            _message_callback(std::bind(&TcpClient::defaultMessageCallback, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3)),
            _connect_done_callback(std::bind(&TcpClient::defalutConnectCallback, this, std::placeholders::_1)),
            _disconnect_callback(std::bind(&TcpClient::defalutDisconnectCallback, this, std::placeholders::_1))
        {}

        TcpClient::~TcpClient()
        {
        }

        void TcpClient::init()
        {
            _connection->setMessageCallback(std::bind(&TcpClient::onMessage, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            _connection->setConnectedCallback(std::bind(&TcpClient::onConnect, this,
                    std::placeholders::_1));
            _connection->setCloseCallback(std::bind(&TcpClient::onDisConnect, this,
                    std::placeholders::_1));
        }

        void TcpClient::stop()
        {

        }
    }
}