#ifndef __EASYNET_EVENT_H__
#define __EASYNET_EVENT_H__

#include "easynet_channel.h"
#include <sys/eventfd.h>
#include <stdint.h>
#include <functional>
#include <list>
#include <algorithm>
#include <assert.h>
#include <unistd.h>
#include "easynet_mutex.h"

namespace easynet
{
    namespace net
    {
        class EventLoop;
        class EventNotify
        {
            public:
                EventNotify(EventLoop *loop)
                    : _event_fd(eventfd(0, 0)),
                _channel(loop, _event_fd)
                {
                    _channel.setReadCallback(std::bind(&EventNotify::handleRead, this));
                    _channel.enableRead();
                }
                ~EventNotify()
                {
                    ::close(_event_fd);
                }
            public:
                void run(const std::function<void()> &functor)
                {
                    {
                        easynet::sys::easynet_scoped_mutex lock(_mutex);
                        _pending_functors.push_back(functor);
                    }
                    notify();
                }
                
            private:
                void notify()
                {
                    int rc;
                    uint64_t one = 1;

                    rc = ::write(_event_fd, &one, sizeof(one));
                    (void)rc;
                }

                void handleRead()
                {
                    uint64_t one = 0;
                    int rc;

                    rc = ::read(_event_fd, &one, sizeof(one));
                    (void)one;
                    assert(rc == sizeof(uint64_t));

                    std::list<std::function<void()> > running;
                    {
                        easynet::sys::easynet_scoped_mutex lock(_mutex);
                        std::swap(_pending_functors, running);
                    }
                    std::for_each(running.begin(), running.end(), 
                        [](const std::function<void()> &f) {f();});
                }
            private:
                int _event_fd;
                Channel _channel;
                easynet::sys::easynet_mutex _mutex;
                std::list<std::function<void()> > _pending_functors;
        };
    }
}

#endif
