#include "easynet_eventloop.h"
#include "easynet_epollpoller.h"
#include <assert.h>

namespace 
{
    easynet::net::Poller *newPoller()
    {
        return new easynet::net::EpollPoller();
    }
}

namespace easynet
{
    namespace net
    {
        EventLoop::EventLoop()
            : _looping(false),
              _stop(true),
              _poller(newPoller()),
              _notify(this),
              _timer_queue(this)

        {
        }

        EventLoop::~EventLoop()
        {
        }

        void EventLoop::start()
        {
            int rc;

            if(_looping) return;
            _looping = true;
            _stop = false;

            for(;;)
            {
                if(_stop) break;
                _fired_channels.clear();
                rc = _poller->poll(_fired_channels, 1000);
                assert(rc >= 0);
                handleFiredChannels();
            }
            _looping = false;
        }

        void EventLoop::stop()
        {
            _stop = true;
        }

        void EventLoop::runFunc(const std::function<void()> &f)
        {
            _notify.run(f);
        }

        void EventLoop::updateChannel(Channel *channel,
            bool write, bool read)
        {
            if(write || read)
            {
                _poller->updateChannel(channel, write, read);
            }
            else {
                _poller->deleteChannel(channel);
            }
        }

        void EventLoop::handleFiredChannels()
        {
            auto it1 = _fired_channels.begin();
            auto it2 = _fired_channels.end();
            int rc;

            for(; it1 != it2; ++it1)
            {
                (*it1)->handleEvent();
            }
        }

        TimerId EventLoop::runAfter(const easynet::net::Timer::TimerFunc &func, uint64_t after)
        {
            _timer_queue.addTimer(func, after, 0, false);
        }

        TimerId EventLoop::runEvery(const easynet::net::Timer::TimerFunc &func, uint64_t every)
        {
            _timer_queue.addTimer(func, every, (uint64_t)-1, true);
        }

        TimerId EventLoop::runEvery(const easynet::net::Timer::TimerFunc &func, uint64_t every, uint32_t times)
        {
            _timer_queue.addTimer(func, every, times, true);
        }

        void EventLoop::removeTimer(const easynet::net::TimerId &id)
        {
            _timer_queue.cancelTimer(id);
        }
    }
}
