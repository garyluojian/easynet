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
		int rc = epoll_ctl(epoll_fd_, channel->fd(), &event);
		if(0 != rc)
		{
			LOG(ERROR)<<"epoll_ctl failed.";
			return -1;
		}
		return 0;
	}
}
