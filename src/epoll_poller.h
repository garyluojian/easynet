#ifndef _EASYNET_POLLER_H_
#define _EASYNET_POLLER_H_

#include "poller.h"
#include <vector>
#include <sys/epoll.h>

namespace easynet
{
class EpollPoller : public Poller
{
public:
	EpollPoller();
	~EpollPoller();

	int addChannel(Channel *channel, bool write, bool read);
	int removeChannel(Channel *channel);
	int updateChannel(Channel *channel, bool write, bool read);
	int getFiredChannel(std::list<Channel*> &fired_lst, uint64_t to);

private:
	int epoll_fd_;
	std::vector<struct epoll_event> fired_events_;
};
}

#endif
