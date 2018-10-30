#include "easynet_thread.h"
#include <gtest/gtest.h>

using namespace easynet::sys;

void test_thread_func(void *args)
{
    int *x = static_cast<int*>(args);

    *x += 5;
}

TEST(llt_easynet_thread, basic_test)
{
    int x = 0;
    easynet_thread thread(test_thread_func, &x, "test_thread");

    thread.start();
    thread.join();

    EXPECT_EQ(5, x);
}

class test_callback
{
public:
    void test_method_callback(void *args)
    {
        int *x = static_cast<int*>(args);

        *x += 5;
    }
};

TEST(llt_easynet_thread, test_for_method)
{
    int x = 0;

    test_callback test_obj;
    easynet_thread thread(std::bind(&test_callback::test_method_callback, 
            &test_obj, std::placeholders::_1), 
            &x, "test_thread");

    thread.start();
    thread.join();

    EXPECT_EQ(5, x);
}

