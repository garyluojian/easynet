cmake_minimum_required(VERSION 3.0)

project(easynet)

include_directories(/usr/include/
        /usr/include/c++/7/
        /usr/local/include/
        ./net/
        ./sys/)

set(NET_SRCS net/easynet_tcpserver.cc
        net/easynet_acceptor.cc
        net/easynet_channel.cc
        net/easynet_tcpconnection.cc
        net/easynet_buffer.cc
        net/easynet_epollpoller.cc
        net/easynet_eventloop.cc
        net/easynet_socket.cc
        net/easynet_timer.cc
        net/easynet_timerid.cc
        net/easynet_tcpclient.cc
        net/easynet_udpserver.cc)

set(SYS_SRCS sys/easynet_condition.cc
        sys/easynet_thread.cc
        sys/easynet_threadpool.cc
        sys/easynet_packetqueue.cc)

add_library(easynet STATIC ${NET_SRCS} ${SYS_SRCS})
target_link_libraries(easynet pthread glog)
