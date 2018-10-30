#ifndef __EASYNET_SOCKET_H__
#define __EASYNET_SOCKET_H__

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

namespace easynet
{
    namespace net
    {
        class Socket
        {
            public:
                enum SOCK_TYPE
                {
                    SOCK_TYPE_TCP = 0,
                    SOCK_TYPE_UDP,
                };
                Socket();
                Socket(SOCK_TYPE type);
                Socket(int fd);
                ~Socket();

            public:
                void close() {if(_fd != -1) ::close(_fd); _fd = -1;}
                int create(Socket::SOCK_TYPE type);
                int fd() {return _fd;}
                void setBlock(bool block);
                void setAddrReuse();
                void setTcpNodelay();
                void setBroadcast(bool on_off);
                void setRecvLowWater(int low_water);
                void setSndLowWater(int low_water);
                void setRecvBufferSize(int buffer_size);
                void setSndBufferSize(int buffer_size);
                void setLinger(int on_off, int linger);
                int getSockError();
                int bind(const std::string &server_ip, unsigned short server_port);
                int listen(int backlog)
                {
                    return ::listen(_fd, backlog);
                }
                int accept(std::string &peer_ip, unsigned short &peer_port);
                int connect(const std::string &peer_ip, unsigned short peer_port);
                
            private:
                int _fd;
        };
    }
}

#endif
