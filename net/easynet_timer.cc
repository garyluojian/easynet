//
// Created by gary on 18-9-8.
//
#include "easynet_timer.h"
#include "easynet_eventloop.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>

namespace easynet
{
    namespace net
    {
        Timer::Timer(const TimerFunc &func, uint64_t interval, uint64_t times, bool repeat)
            : _func(func), _interval(interval), _times(times), _used(0), _repeat(repeat),
            _expired_time(std::chrono::steady_clock::now() + _interval)
        {
        }

        TimerQueue::TimerQueue(EventLoop *loop)
            : _loop(loop),
            _timer_fd(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
            _channel(loop, _timer_fd)
        {
            _channel.setReadCallback(std::bind(&TimerQueue::onRead, this));
            _channel.enableRead();
        }

        TimerQueue::~TimerQueue() {::close(_timer_fd);}

        TimerId TimerQueue::addTimer(const easynet::net::Timer::TimerFunc &func,
                uint64_t interval, uint64_t times, bool repeat)
        {
            std::shared_ptr<Timer> timer(new Timer(func, interval, times, repeat));
            _loop->runFunc(std::bind(&TimerQueue::add2TimerQueue, this, timer));

            return timer->id();
        }

        void TimerQueue::cancelTimer(const easynet::net::TimerId &id)
        {
            _loop->runFunc(std::bind(&TimerQueue::removeFromTimerQueue, this, id));
        }

        void TimerQueue::add2TimerQueue(std::shared_ptr<easynet::net::Timer> timer)
        {
            bool changed = insert(timer);
            if(changed)
            {
                resetTimerfd(timer->_expired_time);
            }
        }

        void TimerQueue::removeFromTimerQueue(const easynet::net::TimerId &id)
        {
            auto it = std::find_if(_timer_queue.begin(), _timer_queue.end(),
                                   [id](const std::shared_ptr<Timer> &t){return t->id() == id;});
            if(it != _timer_queue.end())
            {
                _timer_queue.erase(it);
            }
        }

        bool TimerQueue::insert(std::shared_ptr<easynet::net::Timer> timer)
        {
            bool changed = false;
            std::list<std::shared_ptr<Timer> >::iterator it;

            for(it = _timer_queue.begin(); it != _timer_queue.end(); ++ it)
            {
                if(!(**it < *timer))
                {
                    break;
                }
            }

            if(it == _timer_queue.begin())
            {
                changed = true;
            }
            _timer_queue.insert(it, timer);

            return changed;
        }

        void TimerQueue::resetTimerfd(const std::chrono::steady_clock::time_point &new_tp)
        {
            struct itimerspec new_value;
            struct itimerspec old_value;
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            auto time_span = new_tp - now;

            memset(&new_value, 0, sizeof(new_value));
            memset(&old_value, 0, sizeof(old_value));
            if(time_span.count() > 0)
            {
                new_value.it_value.tv_sec = time_span.count() / std::chrono::steady_clock::period::den;
                new_value.it_value.tv_nsec = time_span.count() % std::chrono::steady_clock::period::den;
            }
            else
            {
                new_value.it_value.tv_nsec = 1;
            }

            (void)::timerfd_settime(_timer_fd, 0, &new_value, &old_value);
        }

        void TimerQueue::onRead()
        {
            uint64_t x;
            ssize_t n = ::read(_timer_fd, &x, sizeof(x));
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::list<std::shared_ptr<Timer> >::iterator e = _timer_queue.end();
            std::list<std::shared_ptr<Timer> >::iterator current;

            for(std::list<std::shared_ptr<Timer> >::iterator b = _timer_queue.begin(); b != e;)
            {
                current = b++;
                std::shared_ptr<Timer> t = *current;
                if(t->_expired_time <= now)
                {
                    _timer_queue.erase(current);
                    t->runTimerFunc();
                    if(t->repeat() && (t->_times == (uint64_t)-1 || t->_times > t->_used))
                    {
                        ++t->_used;
                        t->update();
                        (void)insert(t);
                    }
                }
                else
                {
                    break;
                }
            }
            if(_timer_queue.size() > 0)
            {
                resetTimerfd((*_timer_queue.begin())->_expired_time);
            }
        }
    }
}
