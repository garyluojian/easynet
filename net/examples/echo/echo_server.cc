#include "easynet_tcpserver.h"
#include <iostream>
#include <string>

using namespace easynet::net;
using namespace std;

void onEchoMsg(std::shared_ptr<TcpConnection> conn, const void *data, size_t data_len)
{
    const char *rsp_msg = "Hello, echo_client";
    string recv_data(static_cast<const char*>(data), data_len);
    cout<<"Recv msg: "<<recv_data<<" from "<<conn->getConnectionName()<<endl;
    conn->send(rsp_msg, strlen(rsp_msg));
}

void newConnection(std::shared_ptr<TcpConnection> conn)
{
    cout<<"New connection from "<<conn->getConnectionName()<<endl;
}

int main()
{
    easynet::net::EventLoop loop;
    string server_ip = "127.0.0.1";
    unsigned short server_port = 9877;
    easynet::net::TcpServer server(&loop, server_ip, server_port);

    server.setMessageCallback(onEchoMsg);
    server.setConnectCallback(newConnection);

    if(0 != server.start())
    {
        cerr<<"start echo server failed."<<endl;
    }
    loop.start();

    return 0;
}
