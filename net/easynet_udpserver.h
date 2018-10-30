#ifndef __EASYNET_UDPSERVER_H__
#define __EASYNET_UDPSERVER_H__

#include "easynet_eventloop.h"
#include "easynet_socket.h"
#include "easynet_channel.h"
#include <string>
#include <functional>

namespace easynet
{
    namespace net
    {
        class EventLoop;
        class UdpServer
        {
        public:
            UdpServer(EventLoop *loop, const std::string &ip, unsigned short port)
                : _loop(loop),
                _server_ip(ip),
                _server_port(port),
                _server_socket(Socket::SOCK_TYPE_UDP),
                channel(loop, _server_socket.fd()),
                _message_callback(std::bind(&UdpServer::defaultMessageCallback, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4))
            {
                channel.setReadCallback(std::bind(&UdpServer::onMessage, this));
                channel.enableRead();
            }
            ~UdpServer(){}
            int start();
            void stop();
            int send(const std::string &peer_ip, unsigned short peer_port, const void *data, size_t len);

            void setMessageCallback(
                    const std::function<void(const std::string&, unsigned short, const void*, size_t)> &message_callback
                    )
            {
                _message_callback = message_callback;
            }

        private:
            void onMessage();
            void defaultMessageCallback(const std::string&, unsigned short, const void*, size_t){}

        private:
            EventLoop *_loop;
            std::string _server_ip;
            unsigned short _server_port;
            Socket _server_socket;
            Channel channel;
            std::function<void(const std::string&, unsigned short, const void*, size_t)> _message_callback;
        };
    }
}

#endif