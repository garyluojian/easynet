#include "easynet_tcpconnection.h"
#include "easynet_eventloop.h"
#include "easynet_endian.h"
#include <unistd.h>
#include <assert.h>
#include <glog/logging.h>
#include <string.h>
#include <errno.h>

namespace easynet
{
    namespace net
    {
        const size_t TcpConnection::_output_buffer_chunk_size = 1<<20;
        TcpConnection::TcpConnection(EventLoop *loop)
            : _loop(loop),
            _socket(),
            _channel(loop, _socket.fd()),
            _is_server(false),
            _connection_status(initStatus),
            _output_buffer(_output_buffer_chunk_size)
        {
            init();
        }
        TcpConnection::TcpConnection(EventLoop *loop, int fd)
            : _loop(loop),
             _socket(fd),
             _channel(loop, fd),
             _is_server(true),
             _connection_status(connectedStatus),
             _output_buffer(_output_buffer_chunk_size)
        {
            init();
        }

        void TcpConnection::init()
        {
            _channel.setWriteCallback(std::bind(&TcpConnection::onWrite, this));
            _channel.setReadCallback(std::bind(&TcpConnection::onRead, this));
        }

        TcpConnection::~TcpConnection()
        {
        }

        int TcpConnection::send(const void *data, size_t data_len)
        {
            if(_connection_status != connectedStatus)
            {
                return -1;
            }
            {
                size_t header_size = easynet::sys::host2Net(static_cast<int32_t>(data_len));
                easynet::sys::easynet_scoped_mutex scoped_mutex(_output_buffer_mutex);
                _output_buffer.writeData(&header_size, sizeof(int32_t));
                _output_buffer.writeData(data, data_len);
            }
            _channel.enableWrite();
            return 0;
        }

        void TcpConnection::onRead()
        {
            ssize_t n = 0;
            int i = 2;

            while(i > 0)
            {
                //从网络上读取数据
                --i;
                if(_streamer._state == easynet_tcpstreamer::HEADER)
                {
                    n = ::read(_channel.fd(),
                            _streamer._header + _streamer._offset,
                            _streamer._want);

                    if(n > 0)
                    {
                        if(n == _streamer._want)
                        {
                            _streamer._state = easynet_tcpstreamer::DATA;
                            _streamer._offset = 0;
                            size_t data_len = 0;
                            memcpy(&data_len, _streamer._header, sizeof(_streamer._header));
                            data_len = easynet::sys::net2Host(static_cast<int32_t>(data_len));
                            _streamer._want = data_len;
                            _streamer._data.reserve(data_len);
                        }
                        else
                        {
                            _streamer._offset += n;
                            _streamer._want -= n;
                        }
                    }
                    else if(n == 0)
                    {
                        //连接断开
                        onClose();
                        return;
                    }
                    if(n < 0)
                    {
                        LOG(WARNING)<<"OnRead error: "<<strerror(errno);
                        return;
                    }
                }
                else if(_streamer._state == easynet_tcpstreamer::DATA)
                {
                    n = ::read(_channel.fd(),
                            &(*_streamer._data.begin()) + _streamer._offset,
                            _streamer._want);
                    if(n > 0)
                    {
                        if(n == _streamer._want)
                        {
                            _streamer._state = easynet_tcpstreamer::HEADER;
                            _streamer._want = 4;
                            _streamer._offset = 0;
                            _on_message_callback(shared_from_this(), 
                                    &(*_streamer._data.begin()),
                                    _streamer._data.capacity());
                        }
                        else
                        {
                            _streamer._offset += n;
                            _streamer._want -= n;
                        }
                    }
                    else if(n == 0)
                    {
                        onClose();
                        return;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    //do nothing
                    assert(false);
                }
            }
        }

        void TcpConnection::onWrite()
        {
            if(_connection_status == connectedStatus)
            {
                easynet::sys::easynet_scoped_mutex scoped_mutex(_output_buffer_mutex);
                _output_buffer.write(_channel.fd());
                if(!_output_buffer.empty())
                {
                    return;
                }
                //当所有的数据都已经发出,则关闭channel的可写标志
                _channel.disableWrite();
            }
            else if(_connection_status == connectingStatus)
            {
                connectDone();
            }
            else
            {
                //DO NOTHING
            }
        }

        void TcpConnection::onClose()
        {
            _loop->runFunc(std::bind(&TcpConnection::closeMySelf, shared_from_this()));
        }

        void TcpConnection::closeMySelf()
        {
            disconnect();
            if(_on_close_callback)
            {
                _on_close_callback(shared_from_this());
            }
        }

        void TcpConnection::connectDone()
        {
            //连接完成,可能成功，可能失败
            int error = _socket.getSockError();
            if(0 != error)
            {
                //connect failed. retry
                _connection_status = initStatus;
                _loop->runAfter(std::bind(&TcpConnection::connect, this, _peer_ip, _peer_port), 5000);
                return;
            }
            _connection_status = connectedStatus;
            _channel.disableWrite();
            onConnectDone();
            _channel.enableRead();
        }

        void TcpConnection::connectingTimeout()
        {
            if(_connection_status != connectedStatus)
            {
                LOG(WARNING)<<"Connect to "<<"timeout, retry...";
                _connection_status = initStatus;
                _socket.close();
                _socket.create(Socket::SOCK_TYPE_TCP);
                connect(_peer_ip, _peer_port);
            }
        }

        void TcpConnection::onConnectDone()
        {
            if(_on_connected_callback) _on_connected_callback(shared_from_this());
        }

        void TcpConnection::connect(const std::string &ip, unsigned short port)
        {
            int rc = 0;

            if(_connection_status == connectingStatus
                || _connection_status == connectedStatus)
            {
                LOG(FATAL)<<"Connecting is doing or done.";
                return;
            }
            _peer_ip = ip;
            _peer_port = port;
            if(0 != _socket.create(Socket::SOCK_TYPE_TCP))
            {
                LOG(ERROR)<<"Create sock error.";
                _loop->runFunc(std::bind(&TcpConnection::onConnectDone, this));
                return;
            }
            setPeerAddr(ip, port);
            _channel.setFd(_socket.fd());
            rc = _socket.connect(ip, port);
            switch(rc)
            {
                case Socket::CONN_OK:
                    //connected OK
                    _connection_status = connectedStatus;
                    _loop->runFunc(std::bind(&TcpConnection::onConnectDone, this));
                     _channel.enableRead();
                    break;
                case Socket::CONN_DOING:
                    //connecteding
                    _connection_status = connectingStatus;
                    _channel.enableWrite();
                    _loop->runAfter(std::bind(&TcpConnection::connectingTimeout, this), 5000);
                    break;
                case Socket::CONN_NEED_RETRY:
                    //error, retry
                    if(_is_autoreconn && (_autoreconn_times - _reconned_times > 0))
                    {
                        ++_reconned_times;
                        _loop->runAfter(std::bind(&TcpConnection::connect, this, ip, port), _autoreconn_interval);
                    }
                    else
                    {
                        _loop->runFunc(std::bind(&TcpConnection::onConnectDone, this));
                    }
                    break;
                default:
                    LOG(FATAL)<<"Unexpect error when connect to "<<ip<<":"<<port;
                    break;
            }
        }

        void TcpConnection::disconnect()
        {
            _connection_status = initStatus;
            _channel.disableAll();
            _socket.close();
        }
    }
}
