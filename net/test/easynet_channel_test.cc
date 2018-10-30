#include "easynet_eventloop.h"
#include "easynet_channel.h"
#include <gtest/gtest.h>

using namespace easynet::net;

void llt_write_callback()
{
    EXPECT_EQ(0, 0);
}

void llt_write_callback_for_not_fired()
{
    EXPECT_EQ(1, 0);
}

TEST(llt_easynet_channel, write_event_test)
{
    EventLoop loop;
    Channel channel(&loop, 0);

    channel.enableWrite();
    channel.writeEventFired();
    channel.setWriteCallback(llt_write_callback);

    channel.handleEvent();

    channel.setWriteCallback(llt_write_callback_for_not_fired);

    channel.handleEvent();

    channel.disableWrite();
    channel.writeEventFired();

    channel.handleEvent();
}
