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
                    SOCK_TYPE_UNKNOW,
                };
                enum CONN_ERROR_CODE
                {
                    /*
                     * 连接成功
                     * */
                    CONN_OK,
                    /*
                     * 连接正在进行,不能立即完成
                     * 只有非阻塞的情况下,才会返回此错误码
                     * */
                    CONN_DOING,
                    /*
                     * 连接错误,需要重试
                     * */
                    CONN_NEED_RETRY,
                    /*
                     * 连接错误,重试也无法成功
                     * */
                    CONN_ERROR,
                };
                Socket();
                Socket(SOCK_TYPE type);
                Socket(int fd);
                ~Socket();

            public:
                void close() {if(_fd != -1) ::close(_fd); _fd = -1;}
                void closeRead() {if(_fd != -1) ::shutdown(_fd, SHUT_RD);};
                void closeWrite() {if(_fd != -1) ::shutdown(_fd, SHUT_WR);};
                int create(Socket::SOCK_TYPE type);
                int fd() {return _fd;}
                void setBlock(bool block);
                void setAddrReuse();
                void setTcpNoDelay();
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
                CONN_ERROR_CODE connect(const std::string &peer_ip, unsigned short peer_port);
                
            private:
                int _fd;
                SOCK_TYPE _type;
        };
    }
}

#endif
