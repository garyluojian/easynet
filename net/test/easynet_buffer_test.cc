#include "easynet_buffer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <array>

using namespace std;
using namespace easynet::net;
using namespace testing;

/*
TEST(llt_easynet_buffer, easynet_buffer_test1)
{
    Buffer buffer(16);
    array<char, 17> temp_buffer;

    for(size_t i = 0; i < temp_buffer.size() - 1; ++i)
    {
        temp_buffer[i] = 'a';
    }
    temp_buffer[temp_buffer.size() - 1] = 'b';
    buffer.push_back(temp_buffer.data(), temp_buffer.size());
    array<char, 16> one_bucket;

    buffer.pop_front(one_bucket.data(), one_bucket.size());

    for(auto c : one_bucket)
    {
        EXPECT_EQ('a', c);
    }
    EXPECT_FALSE(buffer.empty());
    buffer.pop_front(one_bucket.data(), 1);
    EXPECT_EQ('b', one_bucket[0]);
    EXPECT_TRUE(buffer.empty());
}

TEST(llt_easynet_buffer, easynet_buffer_test2)
{
    Buffer buffer(16);
    array<char, 17> temp_buffer;
    
    for(auto &c : temp_buffer)
    {
        c = 'a';
    }
    temp_buffer[temp_buffer.size() - 1] = 'b';
    buffer.push_back(temp_buffer.data(), temp_buffer.size());
    
    array<char, 16> one_chunk;

    buffer.pop_front(one_chunk.data(), one_chunk.size());
    for(auto &c : temp_buffer)
    {
        c = 'c';
    }
    temp_buffer[temp_buffer.size() - 1] = 'd';
    buffer.push_back(temp_buffer.data(), temp_buffer.size());
    buffer.pop_front(temp_buffer.data(), 10);
    EXPECT_EQ('b', temp_buffer[0]);

    for(size_t i = 1; i < 10; ++i)
    {
        EXPECT_EQ('c', temp_buffer[i]);
    }
    buffer.pop_front(temp_buffer.data(), 8);
    for(size_t i = 0; i < 7; ++i)
    {
        EXPECT_EQ('c', temp_buffer[i]);
    }
    EXPECT_EQ('d', temp_buffer[7]);
    EXPECT_TRUE(buffer.empty());
}

TEST(llt_easynet_buffer, easynet_buffer_except_test)
{
    Buffer buffer(16);
    char c_buffer[20];

    EXPECT_NE(0, buffer.pop_front(c_buffer, 10));
    for(size_t i = 0; i < sizeof(c_buffer) / sizeof(c_buffer[0]); ++i)
    {
        c_buffer[i] = 'a';
    }
    buffer.push_back(c_buffer, 18);
    EXPECT_NE(0, buffer.pop_front(c_buffer, 20));
}
*/