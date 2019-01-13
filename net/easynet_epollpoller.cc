#include "easynet_epollpoller.h"
#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <glog/logging.h>
#include <string.h>

namespace
{
    int epollfd_create()
    {
        int fd = epoll_create1(EPOLL_CLOEXEC);
        assert(fd >= 0);
        return fd;
    }
}
namespace easynet
{
    namespace net
    {
        EpollPoller::EpollPoller()
            : _epoll_fd(epollfd_create())
        {
            _fired_channels.reserve(10);
        }

        EpollPoller::~EpollPoller()
        {
            ::close(_epoll_fd);
        }

        void EpollPoller::updateChannel(Channel *c, bool write, bool read)
        {
            int rc;
            struct epoll_event event;

            assert(c);
            event.events = 0;
            if(write) event.events |= EPOLLOUT;
            if(read) event.events |= EPOLLIN;
            event.data.ptr = static_cast<void*>(c);

            rc = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, c->fd(), &event);
            if(0 != rc && errno == EEXIST)
            {
                rc = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, c->fd(), &event);
            }
            LOG_IF(FATAL, 0 != rc)<<"epoll_ctrl error, event = "<<event.events<<", errorcode = "<<strerror(errno);
        }

        void EpollPoller::deleteChannel(Channel *c)
        {
            int rc;
            struct epoll_event event;

            assert(c);
            rc = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, c->fd(), &event);
            //LOG_IF(FATAL, 0 != rc)<<"delete channel failed, "<<c->fd()<<" "<<strerror(errno);
        }

        int EpollPoller::poll(std::vector<Channel*> &fired_channels,
                int timeout)
        {
            int n = 0;
            int capacity = _fired_channels.capacity();
            struct epoll_event *fired_events = &(*_fired_channels.begin());
            Channel *channel = NULL;

            n = epoll_wait(_epoll_fd, fired_events, 
                    capacity, timeout);
            if(n < 0)
            {
                if(errno == EINTR)
                {
                    return 0;
                }
                return -1;
            }

            for(int i = 0; i < n; ++i)
            {
                channel = static_cast<Channel*>(_fired_channels[i].data.ptr);

                if(_fired_channels[i].events & EPOLLIN)
                {
                    channel->readEventFired();
                    fired_channels.push_back(channel);
                }
                if(_fired_channels[i].events & EPOLLOUT)
                {
                    channel->writeEventFired();
                    fired_channels.push_back(channel);
                }
                if(_fired_channels[i].events & EPOLLRDHUP)
                {
                    channel->closeEventFired();
                    fired_channels.push_back(channel);
                }
                if(_fired_channels[i].events & (EPOLLERR | EPOLLHUP))
                {
                    channel->errorEventFired();
                    fired_channels.push_back(channel);
                }
                //other events ?
            }

            if(n == capacity)
            {
                _fired_channels.reserve(capacity<<1);
            }

            return 0;
        }
    }
}
