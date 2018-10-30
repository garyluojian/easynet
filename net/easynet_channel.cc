#include "easynet_eventloop.h"
#include "easynet_channel.h"

namespace easynet
{
    namespace net
    {
        Channel::Channel(
            EventLoop *loop, int fd)
            : _loop(loop), _fd(fd),
            _fired_events(0),
            _monitor_events(0),
            _read_callback(std::bind(&Channel::defaultReadCallback, 
                this)),
            _write_callback(std::bind(&Channel::defaultWriteCallback,
                this)),
            _close_callback(std::bind(&Channel::defaultCloseCallback,
                this))
        {
        }

        Channel::~Channel()
        {

        }

        void Channel::handleEvent()
        {
            if(_fired_events & read_event)
            {
               _read_callback();
            }
            if(_fired_events & write_event)
            {
                _write_callback();
            }
            if(_fired_events & (close_event | error_event))
            {
                _close_callback();
            }
            _fired_events = invalid_event;
        }

        void Channel::update()
        {
            _loop->updateChannel(this, _monitor_events & write_event,
                _monitor_events & read_event);
        }
    }
}
