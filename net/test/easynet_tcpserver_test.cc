//
// Created by gary on 18-10-21.
//

#include "easynet_eventloop.h"
#include "easynet_tcpserver.h"
#include "easynet_tcpconnection.h"
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
using namespace easynet::net;

static void onNewConnection(std::shared_ptr<TcpConnection> new_connection)
{
    cout<<"New connection "<<new_connection->getConnectionName()<<" in"<<endl;
}

static void onMessage(std::shared_ptr<TcpConnection> connection,
        const void *data, size_t data_len)
{
    string message(static_cast<const char*>(data), data_len);
    cout<<"Recv "<<message<<" from "<<connection->getConnectionName()<<endl;
}

static void initServerAddr(struct sockaddr_in *server_addr, const string &ip, unsigned short port)
{
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    (void)inet_pton(AF_INET, ip.c_str(), &server_addr->sin_addr);
    server_addr->sin_port = htons(port);
}
static void clientConnect()
{
    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr1, server_addr2;

    initServerAddr(&server_addr1, "127.0.0.1", 8098);
    initServerAddr(&server_addr2, "127.0.0.1", 8099);

    int rc = connect(fd1, (struct sockaddr*)&server_addr1, sizeof(server_addr1));
    rc = connect(fd2, (struct sockaddr*)&server_addr2, sizeof(server_addr2));
    EXPECT_EQ(0, rc);

    sleep(1);
    //send some message
    for(int i = 0; i < 10; ++i) {
        char *data1 = "hello world to server1";
        char *data2 = "hello world to server2";
        char header[4] = {0};
        int len = strlen(data1);
        memcpy(header, &len, sizeof(len));
        rc = ::send(fd1, header, 4, 0);
        rc = ::send(fd1, data1, strlen(data1), 0);
        EXPECT_EQ(strlen(data1), rc);
        rc = ::send(fd2, header, 4, 0);
        rc = ::send(fd2, data2, strlen(data2), 0);
        EXPECT_EQ(strlen(data2), rc);
    }
    ::close(fd1);
    ::close(fd2);
}

static void showClientCount(TcpServer *server)
{
    EXPECT_EQ(0, server->getAllConnection().size());
    cout<<"Online client count: "<<server->getAllConnection().size()<<endl;
    server->stop();
}

TEST(llt_tcpserver, tcpserver_start)
{
    EventLoop loop;
    TcpServer server1(&loop, "127.0.0.1", 8098);
    TcpServer server2(&loop, "127.0.0.1", 8099);

    server1.setConnectCallback(onNewConnection);
    server1.setMessageCallback(onMessage);
    if(-1 == server1.start())
    {
        cout<<"Start tcp server failed, errormsg: "<<strerror(errno)<<endl;
        return;
    }
    server2.setConnectCallback(onNewConnection);
    server2.setMessageCallback(onMessage);
    if(-1 == server2.start())
    {
        cout<<"Start tcp server failed, errormsg: "<<strerror(errno)<<endl;
        return;
    }
    loop.runAfter(clientConnect, 2000);
    loop.runAfter(std::bind(showClientCount, &server1), 5000);
    loop.start();
}
