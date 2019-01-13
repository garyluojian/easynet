#ifndef __EASYNET_CHANNEL_H__
#define __EASYNET_CHANNEL_H__

#include <functional>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        class EventLoop;
        class Channel
        {
            public:
                Channel(EventLoop *loop, int fd);
                ~Channel();

            public:
                int fd() {return _fd;}
                void setFd(int fd) {_fd = fd;};
                void readEventFired() 
                {
                    if(_monitor_events & read_event)
                    {
                        _fired_events |= read_event;
                    }
                }
                void writeEventFired() 
                {
                    if(_monitor_events & write_event)
                    {
                        _fired_events |= write_event;
                    }
                }
                void closeEventFired() {_fired_events |= close_event;}
                void errorEventFired() {_fired_events |= error_event;}

                void enableRead()
                {
                    if(!(_monitor_events & read_event))
                    {
                        _monitor_events |= read_event;
                        update();
                    }
                }
                void enableWrite() 
                {
                    if(!(_monitor_events & write_event)) 
                    {
                        _monitor_events |= write_event;
                        update();
                    }
                }

                void disableRead()
                {
                    if(_monitor_events & read_event)
                    {
                        _monitor_events &= ~read_event;
                        update();
                    }
                }
                void disableWrite()
                {
                    if(_monitor_events & write_event)
                    {
                        _monitor_events &= ~write_event;
                        update();
                    }
                }
                void disableAll()
                {
                    _monitor_events &= ~read_event;
                    _monitor_events &= ~write_event;
                    update();
                }

                void setReadCallback(const std::function<void()> &read_callback)
                {
                    _read_callback = read_callback;
                }

                void setWriteCallback(const std::function<void()> &write_callback)
                {
                    _write_callback = write_callback;
                }

                void setCloseCallback(const std::function<void()> &close_callback)
                {
                    _close_callback = close_callback;
                }

                void handleEvent();

            private:
                enum
                {
                    invalid_event = 0,
                    read_event = 1<<0,
                    write_event = 1<<1,
                    close_event = 1<<2,
                    error_event = 1<<3,
                };

            private:
                void update();
                void defaultReadCallback()
                {
                    LOG(INFO)<<"defaultReadCallback";
                }
                void defaultWriteCallback()
                {
                    LOG(INFO)<<"defaultWriteCallback";
                }
                void defaultCloseCallback()
                {
                    LOG(INFO)<<"defaultCloseCallback";
                }

            private:
                EventLoop *_loop;
                int _fd;
                int _fired_events;
                int _monitor_events;
                std::function<void ()> _read_callback;
                std::function<void ()> _write_callback;
                std::function<void ()> _close_callback;
        };
    }
}

#endif
