#include "easynet_tcpclient.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include "easynet_thread.h"

using namespace easynet::net;
using namespace easynet::sys;
using namespace std;

void onEchoRspMsg(std::shared_ptr<TcpConnection> &conn, const void *data, size_t len)
{
    string recv_data(static_cast<const char*>(data), len);

    cout<<"Recv echo rsp msg: "<<recv_data<<" from "<<conn->getConnectionName()<<endl;
}

void onConnectDone(std::shared_ptr<TcpConnection> &conn)
{
    cout<<"Connect to "<<conn->getConnectionName()<<" OK."<<endl;
}

void echo(void *ctx)
{
    TcpClient *client = (TcpClient*)ctx;
	const char *msg = "Hello echo_server";
	while(1)
	{
		client->send(msg, strlen(msg));
		sleep(1);
	}
}

int main()
{
    EventLoop loop;
    string server_ip = "127.0.0.1";
    unsigned short server_port = 9877;
    TcpClient client(&loop, server_ip, server_port);

    client.setMessageCallback(onEchoRspMsg);
    client.setConnectionCallback(onConnectDone);

    client.init();
    client.connect();

    easynet_thread echo_thread(echo, &client, "echo_thread");
    echo_thread.start();

    loop.start();
    echo_thread.join();

    return 0;
}
