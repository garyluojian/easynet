#include "easynet_udpserver.h"

namespace easynet
{
    namespace net
    {
        int UdpServer::start()
        {
            return _server_socket.bind(_server_ip, _server_port);
        }

        void UdpServer::stop()
        {

        }

        int UdpServer::send(const std::string &peer_ip, unsigned short peer_port, const void *data, size_t len)
        {
            return len;
        }

        void UdpServer::onMessage()
        {
            //recv data from net, and call message callback

        }
    }
}