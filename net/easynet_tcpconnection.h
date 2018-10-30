#ifndef __EASYNET_TCPCONNECTION_H__
#define __EASYNET_TCPCONNECTION_H__

#include "easynet_channel.h"
#include "easynet_buffer.h"
#include "easynet_mutex.h"
#include "easynet_socket.h"
#include <string>
#include <functional>
#include <memory>

namespace easynet
{
    namespace net
    {
        class EventLoop;

        class TcpConnection : public std::enable_shared_from_this<TcpConnection>
        {
            public:
                TcpConnection(EventLoop *loop);
                TcpConnection(EventLoop *loop, int fd);
                ~TcpConnection();

            public:
                int send(const void *data, size_t data_len);
                void setAutoReconn(bool on_off, int interval, int times)
                {
                    _is_autoreconn = on_off;
                    _autoreconn_interval = interval;
                    _autoreconn_times = times;
                }
                void connect(const std::string &ip, unsigned short port);
                void disconnect();
                bool connected()
                {
                    return _connection_status == connectedStatus;
                }
                void setMessageCallback(
                    const std::function<void(std::shared_ptr<TcpConnection>,const void*,size_t)> &message_callback)
                {
                    _on_message_callback = message_callback;
                }
                void setCloseCallback(const std::function<void(std::shared_ptr<TcpConnection>)> &close_callback)
                {
                    _on_close_callback = close_callback;
                }
                void setConnectedCallback(const std::function<void(std::shared_ptr<TcpConnection>)> &connected_callback)
                {
                    _on_connected_callback = connected_callback;
                }

                void setPeerAddr(const std::string &peer_ip, unsigned short peer_port)
                {
                    _peer_ip = peer_ip;
                    _peer_port = peer_port;

                    char str_port[8] = {0};
                    snprintf(str_port, sizeof(str_port) / sizeof(str_port[0]), "%u", _peer_port);
                    _connection_name = _peer_ip + ":" + str_port;
                }

                void enableRead()
                {
                    _channel.enableRead();
                }

                const std::string &getConnectionName() const
                {
                    return _connection_name;
                }

        private:
                void init();
                void onRead();
                void onWrite();
                void onClose();
                void connectDone();
                void onConnectDone();
                void closeMySelf();
                void connectingTimeout();
                enum
                {
                    initStatus,
                    connectingStatus,
                    connectedStatus,
                };
            private:
                EventLoop *_loop;
                Socket _socket;
                std::string _peer_ip;
                unsigned short _peer_port;
                std::string _connection_name;
                Channel _channel;
                bool _is_server;
                int _connection_status;
                easynet::sys::easynet_mutex _output_buffer_mutex;
                Buffer _output_buffer;
                bool _is_autoreconn;
                int _autoreconn_interval;
                int _autoreconn_times;
                int _reconned_times;
                std::function<void (std::shared_ptr<TcpConnection>,const void*,size_t)> _on_message_callback;
                std::function<void(std::shared_ptr<TcpConnection>)> _on_close_callback;
                std::function<void (std::shared_ptr<TcpConnection>)> _on_connected_callback;
                struct easynet_tcpstreamer
                {
                    enum
                    {
                        HEADER,
                        DATA
                    };
                    int _state;
                    char _header[4];
                    std::vector<char> _data;
                    size_t _want;
                    size_t _offset;
                    easynet_tcpstreamer()
                    : _state(HEADER),
                    _want(4),
                    _offset(0)
                    {
                    }
                }_streamer;
                static const size_t _output_buffer_chunk_size;
        };
    }
}

#endif
