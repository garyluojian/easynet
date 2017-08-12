#include "epoll_poller.h"
#include <glog/logging.h>

namespace
{
	int createEpollFd()
	{
		int fd;

		fd = epoll_create1(EPOLL_CLOEXEC);
		if(fd < 0)
		{
			LOG(FINAL)<<"epoll_create1 failed.";
		}

		return fd;
	}
}
namespace easynet
{
	EpollPoller::EpollPoller()
	{
		epoll_fd_ = createEpollFd();
	}

	EpollPoller::~EpollPoller()
	{
		::close(epoll_fd_);
	}

	int EpollPoller::addChannel(Channel *channel, bool write, bool read)
	{
		struct epoll_event event;

		event.events = 0;
		if(write)
		{
			event.events |= EPOLLOUT;
		}
		if(read)
		{
			event.events |= EPOLLIN;
		}
		event.data.ptr = channel;
		int rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &event);
		if(0 != rc)
		{
			LOG(ERROR)<<"epoll_ctl add failed("<<strerror(errno)<<").";
			return -1;
		}
		return 0;
	}

	int EpollPoller::removeChannel(Channel *channel)
	{
		struct epoll_event event;

		int rc = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, channel->fd(), &event);
		if(0 != rc)
		{
			LOG(ERROR)<<"epoll_ctl del failed("<<strerror(errno)<<").";
			return -1;
		}
		return 0;
	}

	int EpollPoller::updateChannel(Channel *channel, bool write, bool read)
	{
		struct epoll_event event;

		event.events = 0;
		if(write)
		{
			event.events |= EPOLLOUT;
		}
		if(read)
		{
			event.events |= EPOLLIN;
		}
		event.data.ptr = channel;
		int rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &event);
		if(0 != rc)
		{
			LOG(ERROR)<<"epoll_ctl mod failed("<<strerror(errno)<<").";
			return -1;
		}
		return 0;
	}

	int EpollPoller::getFiredChannel(
			std::list<Channel*> &fired_lst,
			uint64_t to)
	{
		int n = 0;
		int capacity = static_cast<int>(fired_events_.capacity());
		int rc = 0;

		fired_events_.clear();
		n = epoll_wait(epoll_fd_,
				&(*fired_events_.begin()),
				capacity,
				to);
		if(n < 0)
		{
			if(errno == EINTR)
			{
				LOG(WARN)<<"errno = EINTR";
				n = 0;
			}
			else
			{
				LOG(ERROR)<<"strerror = "<<strerror(errno);
				rc = -1;
			}
		}

		auto f = [&fired_lst](std::vector<struct epoll_event>::iterator it)
				{
					Channel *c = static_cast<Channel*>(it->data.ptr);
					if(!c) return;
					c->setFiredEvents(it->events);
					fired_lst.push_back(c);
				};
		for_each(fired_events_.begin(), fired_events_.begin() + n, f);
		if(n == capacity)
		{
			fired_lst.resize(capacity<<1);
		}

		return rc;
	}
}
