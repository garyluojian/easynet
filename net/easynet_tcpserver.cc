#include "easynet_tcpserver.h"
#include <algorithm>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        TcpServer::TcpServer(EventLoop *loop, 
            const std::string &server_ip,
            unsigned int server_port)
            : _loop(loop),
            _acceptor(new Acceptor(loop, server_ip, server_port))
        {
            _acceptor->setNewConnectionCallback(std::bind(&TcpServer::onConnectedCallback, this, std::placeholders::_1));
            _acceptor->setCloseConnectionCallback(std::bind(&TcpServer::onCloseCallback, this, std::placeholders::_1));
        }

        int TcpServer::start()
        {
            int rc;

            _connections.clear();
            rc = _acceptor->init();
            LOG(INFO)<<"Tcpserver start OK.";
            return rc;
        }

        void TcpServer::stop()
        {
            _acceptor->stop();
            std::for_each(_connections.begin(), _connections.end(),
                [](const std::pair<std::string, std::shared_ptr<TcpConnection> > &connection_pair)
                {connection_pair.second->disconnect();});
            _connections.clear();
            _loop->stop();
        }

        void TcpServer::setMessageCallback(
            const std::function<void (std::shared_ptr<TcpConnection>, const void *, size_t)> &message_callback)
        {
            _acceptor->setReadCallback(message_callback);
        }

        void TcpServer::onConnectedCallback(
            std::shared_ptr<TcpConnection> connection)
        {
            const std::string connection_name = connection->getConnectionName();

            assert(_connections.find(connection_name) == _connections.end());
            _connections[connection_name] = connection;
            if(_onConnectedCallback) _onConnectedCallback(connection);
        }

        void TcpServer::onCloseCallback(std::shared_ptr<TcpConnection> connection)
        {
            const std::string connection_name = connection->getConnectionName();
            auto connection_iter = _connections.find(connection_name);

            if(connection_iter != _connections.end())
            {
                _connections.erase(connection_iter);
                LOG(INFO)<<"Connection "<<connection_name<<" closed.";
            }
        }

        void TcpServer::showAllConnection() const
        {
            for(auto &i : _connections)
            {
                LOG(INFO)<<"Connections, remote addr is: "<<i.first;
            }
        }
    }
}