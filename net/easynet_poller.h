#ifndef __EASYNET_POLLER_H__
#define __EASYNET_POLLER_H__

#include <vector>
#include "easynet_channel.h"

namespace easynet
{
    namespace net
    {
        class Poller
        {
            public:
                virtual ~Poller(){}
                virtual void updateChannel(Channel *c, bool write, bool read) = 0;
                virtual void deleteChannel(Channel *c) = 0;
                virtual int poll(std::vector<Channel*> &channels, int timeout) = 0;
        };
    }
}

#endif
