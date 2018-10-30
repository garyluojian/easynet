#ifndef __EASYNET_TCPSERVER_H__
#define __EASYNET_TCPSERVER_H__

#include "easynet_eventloop.h"
#include "easynet_tcpconnection.h"
#include "easynet_acceptor.h"
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace easynet
{
    namespace net
    {
        class TcpServer
        {
            public:
                TcpServer(EventLoop *loop, const std::string &server_ip, unsigned int server_port);
                ~TcpServer(){}
                TcpServer(const TcpServer &) = delete;
                TcpServer &operator=(const TcpServer&) = delete;
                TcpServer(const TcpServer &&) = delete;

            public:
                int start();
                void stop();
                void setMessageCallback(
                    const std::function<void (std::shared_ptr<TcpConnection>, const void *, size_t)> &message_callback);
                void setConnectCallback(const std::function<void (std::shared_ptr<TcpConnection>)> &f)
                {
                    _onConnectedCallback = f;
                }
                void onConnectedCallback(std::shared_ptr<TcpConnection> connection);
                void onCloseCallback(std::shared_ptr<TcpConnection> connection);
                const std::map<std::string, std::shared_ptr<TcpConnection> > &getAllConnection() const
                {
                    return _connections;
                }
                void showAllConnection() const;
            private:
                EventLoop *_loop;
                std::shared_ptr<Acceptor> _acceptor;
                std::map<std::string, std::shared_ptr<TcpConnection> > _connections;
                std::function<void (std::shared_ptr<TcpConnection>)> _onConnectedCallback;
        };
    }
}

#endif
