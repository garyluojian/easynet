//
// Created by gary on 18-9-8.
//

#ifndef EASYNET_EASYNET_TIMER_H
#define EASYNET_EASYNET_TIMER_H

#include "easynet_timerid.h"
#include "easynet_channel.h"
#include <functional>
#include <chrono>
#include <list>
#include <memory>

namespace easynet
{
    namespace net
    {
        class Timer
        {
            friend class TimerQueue;
            friend bool operator==(const Timer &, const Timer &);
            friend  bool operator<(const Timer&, const Timer&);
        public:
            typedef std::function<void()> TimerFunc;

            Timer(const TimerFunc &func, uint64_t interval, uint64_t times, bool repeat);
            Timer(const Timer&) = delete;
            Timer &operator=(const Timer &) = delete;

            void runTimerFunc() {
                _func();
            }

            void update(){_expired_time = std::chrono::steady_clock::now() + _interval;}
            bool expired(){return std::chrono::steady_clock::now() >= _expired_time;}
            bool repeat() {return _repeat;}
            const TimerId &id() const {return _id;}

        private:
            TimerId _id;
            TimerFunc _func;
            std::chrono::milliseconds _interval;
            uint64_t _times;
            uint64_t _used;
            bool _repeat;
            std::chrono::steady_clock::time_point _expired_time;
        };

        inline bool operator==(const Timer &l, const Timer &r)
        {
            return l._id == r._id;
        }

        inline bool operator<(const Timer &l, const Timer &r)
        {
            return (l._expired_time < r._expired_time)
                || ((l._expired_time == r._expired_time) && l._id < r._id);
        }

        class EventLoop;
        class TimerQueue
        {
        public:
            TimerQueue(EventLoop *loop);
            ~TimerQueue();
            TimerQueue(const TimerQueue&) = delete;
            TimerQueue &operator=(const TimerQueue&) = delete;

            TimerId addTimer(const Timer::TimerFunc &func, uint64_t interval, uint64_t times, bool repeat);
            void cancelTimer(const TimerId &id);
        private:
            void onRead();
            void add2TimerQueue(std::shared_ptr<Timer> timer);
            void removeFromTimerQueue(const TimerId &id);
            bool insert(std::shared_ptr<Timer> timer);
            void resetTimerfd(const std::chrono::steady_clock::time_point &new_tp);
        private:
            EventLoop *_loop;
            int _timer_fd;
            Channel _channel;
            std::list<std::shared_ptr<Timer> > _timer_queue;
        };
    }
}

#endif //EASYNET_EASYNET_TIMER_H
