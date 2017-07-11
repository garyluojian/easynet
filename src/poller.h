#pragma once

#include <list>
#include <stdint.h>

using namespace std;

typedef easynet::Channel Channel;

namespace easynet
{
class Poller
{
public:
	virtual ~Poller();
	virtual int addChannel(Channel *c, uint32_t event) = 0;
	virtual int removeChannel(Channel *c) = 0;
	virtual int updateChannel(Channel *c, uint32_t event) = 0;
	virtual int getFiredChannel(list<Channel*> &channel_lst, uint64_t to) = 0;
};
}