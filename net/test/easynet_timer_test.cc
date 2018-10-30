//
// Created by gary on 18-10-4.
//

#include "easynet_eventloop.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace easynet::net;
using namespace testing;
using namespace std;

int i = 0;
void loop_timer_func1()
{
    cout<<"Loop Func1 "<<endl;
    i++;
}

void loop_timer_func2()
{
    cout<<"Loop Func2 "<<endl;
    i++;
}

void loop_timer_func3(EventLoop *loop)
{
    cout<<"Loop Func3"<<endl;
    i++;
    loop->stop();
}

TEST(llt_easynet_timer, loop_timer_test)
{
    EventLoop loop;

    TimerId id1 = loop.runEvery(loop_timer_func1, 1000);
    TimerId id2 = loop.runEvery(loop_timer_func2, 3000);
    TimerId id3 = loop.runEvery(bind(loop_timer_func3, &loop), 5000);
    loop.start();
    EXPECT_TRUE(7 >= i);
}

void once_timer_func(EventLoop *loop)
{
    i++;
    loop->stop();
}

TEST(llt_easynet_timer, once_timer_test)
{
    EventLoop loop;

    i = 0;
    loop.runAfter(bind(once_timer_func, &loop), 2000);
    loop.start();

    EXPECT_EQ(1, i);
}

void times_timer_test(EventLoop *loop)
{
    i++;
}

void times_timer_stop(EventLoop *loop)
{
    loop->stop();
}

TEST(llt_easynet_timer, times_timer_test)
{
    EventLoop loop;

    i = 0;
    loop.runEvery(bind(times_timer_test, &loop), 500, 10);
    loop.runEvery(bind(times_timer_stop, &loop), 5100);
    loop.start();

    EXPECT_EQ(10, i);
}

void cancel_timer_func(EventLoop *loop, const TimerId &id)
{
    loop->removeTimer(id);
}

TEST(llt_easynet_timer, cancel_timer_test)
{
    EventLoop loop;

    i = 0;
    TimerId id = loop.runEvery(bind(times_timer_test, &loop), 500, 10);
    loop.runEvery(bind(cancel_timer_func, &loop, id), 3700);
    loop.runEvery(bind(times_timer_stop, &loop), 4800);

    loop.start();

    EXPECT_EQ(7, i);
}
