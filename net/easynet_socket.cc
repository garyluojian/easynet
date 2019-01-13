#include "easynet_socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        Socket::Socket()
            : _fd(-1),
            _type(SOCK_TYPE_UNKNOW)
        {      
        }

        Socket::Socket(easynet::net::Socket::SOCK_TYPE type)
        {
            _fd = -1;
            int rc = create(type);
            LOG_IF(FATAL, rc != 0)<<"Create socket for type "<<type<<" failed.";
        }

        Socket::Socket(int fd)
            : _fd(fd)
        {
        }

        Socket::~Socket()
        {
            close();
        }

        int Socket::create(easynet::net::Socket::SOCK_TYPE type)
        {
            if(_fd != -1)
            {
                return -1;
            }
            switch(type)
            {
                case SOCK_TYPE_TCP:
                    _fd = socket(AF_INET, SOCK_STREAM, 0);
                    break;
                case SOCK_TYPE_UDP:
                    _fd = socket(AF_INET, SOCK_DGRAM, 0);
                    break;
                default:
                    break;
            }
            if(_fd >= 0)
            {
                return 0;
            }
            return -1;
        }

        void Socket::setBlock(bool block)
        {
            int flags = fcntl(_fd, F_GETFL);

            if(flags < 0)
            {
                assert(false);
            }

            if(block)
            {
                flags &= ~O_NONBLOCK;
            }
            else
            {
                flags |= O_NONBLOCK;
            }
            int rc = fcntl(_fd, F_SETFL, flags);

            if(rc < 0)
            {
                assert(false);
            }
        }

        void Socket::setAddrReuse()
        {
            int rc;
            int on = 1;

            rc = setsockopt(_fd, SOL_SOCKET, 
                    SO_REUSEADDR, &on, sizeof(on));
            if(rc < 0) assert(false);
        }

        void Socket::setTcpNoDelay()
        {
            int rc;
            int off = 1;

            rc = setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
            if(rc < 0) assert(false);
        }

        void Socket::setLinger(int on_off, int linger_timeout)
        {
            int rc;
            struct linger l;

            l.l_onoff = on_off;
            l.l_linger = linger_timeout;

            rc = setsockopt(_fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
            if(rc < 0) assert(false);
        }

        int Socket::getSockError()
        {
            int rc;
            int error;
            socklen_t error_size = sizeof(error);

            rc = getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, &error_size);
            if(rc < 0) assert(false);

            return error;
        }

        int Socket::bind(
            const std::string &server_ip, 
            unsigned short server_port)
        {
            struct sockaddr_in server_addr;

            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            (void)inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
            server_addr.sin_port = htons(server_port);

            int rc = ::bind(_fd, 
                (struct sockaddr*)&server_addr, sizeof(server_addr));
            
            return rc;
        }

        int Socket::accept(std::string &ip, unsigned short &port)
        {
            struct sockaddr_in addr;
            socklen_t sock_len = sizeof(addr);

            memset(&addr, 0, sizeof(addr));
            int fd = ::accept(_fd, (struct sockaddr*)&addr, &sock_len);

            if(fd < 0)
            {
                return -1;
            }
            char ip_addr[16] = {0};

            (void)inet_ntop(AF_INET, &addr.sin_addr, ip_addr, sizeof(ip_addr));
            ip = ip_addr;
            port = ntohs(addr.sin_port);

            return fd;
        }

        Socket::CONN_ERROR_CODE Socket::connect(const std::string &peer_ip, unsigned short peer_port)
        {
            int rc;
            CONN_ERROR_CODE err_code = CONN_OK;
            struct sockaddr_in svr_addr;

            memset(&svr_addr, 0, sizeof(svr_addr));
            svr_addr.sin_family = AF_INET;
            svr_addr.sin_port = htons(peer_port);
            inet_pton(AF_INET, peer_ip.c_str(), &svr_addr.sin_addr);
            rc = ::connect(_fd, (struct sockaddr*)&svr_addr, (socklen_t)sizeof(svr_addr));
            if(rc != 0)
            {
                switch(errno)
                {
                    case EINPROGRESS:
                        err_code = CONN_DOING;
                        break;
                    case EINTR:
                    case ETIMEDOUT:
                    case ECONNREFUSED:
                    case EAGAIN:
                        rc = CONN_NEED_RETRY;
                    default:
                        LOG(FATAL)<<"connect to "<<peer_ip<<" "<<peer_port<<" failed. err_msg: "<<strerror(errno);
                        rc = CONN_ERROR;
                }
            }

            return err_code;
        }
    }
}
