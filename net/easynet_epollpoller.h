#ifndef __EASYNET_EPOLL_POLLER_H__
#define __EASYNET_EPOLL_POLLER_H__

#include "easynet_poller.h"
#include <sys/epoll.h>

namespace easynet
{
    namespace net
    {
        class EpollPoller : public Poller
        {
            public:
                EpollPoller();
                ~EpollPoller();
                void updateChannel(Channel *c, bool write, bool read);
                void deleteChannel(Channel *c);
                int poll(std::vector<Channel*> &, int timeout);
            private:
                int _epoll_fd;
                std::vector<struct epoll_event> _fired_channels;
        };
    }
}

#endif
