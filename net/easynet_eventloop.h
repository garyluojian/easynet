#ifndef __EASYNET_EVENT_LOOP_H__
#define __EASYNET_EVENT_LOOP_H__

#include "easynet_channel.h"
#include "easynet_eventnotify.h"
#include "easynet_poller.h"
#include <vector>
#include <memory>
#include "easynet_timerid.h"
#include "easynet_timer.h"

namespace easynet
{
namespace net
{
    class EventLoop
    {
        public:
            EventLoop();
            ~EventLoop();

            void start();
            void stop();
            void runFunc(const std::function<void()> &f);

            //定时器相关
            TimerId runAfter(const Timer::TimerFunc &func, uint64_t after);
            TimerId runEvery(const Timer::TimerFunc &func, uint64_t every);
            TimerId runEvery(const Timer::TimerFunc &func, uint64_t every, uint32_t times);
            void removeTimer(const TimerId &id);

            void updateChannel(Channel *channel, 
                bool write, bool read);

        private:
            void handleFiredChannels();

        private:
            bool _looping;
            bool _stop;
	        std::shared_ptr<Poller> _poller;
            std::vector<Channel*> _fired_channels;
            EventNotify _notify;
            TimerQueue _timer_queue;
    };
}
}

#endif
