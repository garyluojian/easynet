#ifndef _EASY_NET_CHANNEL_H_
#define _EASY_NET_CHANNEL_H_

#include <stdint.h>
#include <functional>

namespace easynet
{
class Channel
{
public:
	typedef function<void()> MessageCallback;
	Channel(int fd);
	~Channel();

public:
	void setFiredEvents(uint32_t events);
	int fd() {return fd_;}

	void setReadCallback(MessageCallback cb) {read_callback_ = cb;}
	void setWriteCallback(MessageCallback cb) {write_callback_ = cb;}

	void enableRead();
	void enableWrite();

	void disableRead();
	void disableWrite();

private:
	int fd_;
	bool is_write_fired_;
	bool is_read_fired_;

	bool is_write_on_;
	bool is_read_on_;

	MessageCallback read_callback_;
	MessageCallback write_callback_;

	/*
	 * 每一个channel需要有一个输入缓冲区和输出缓冲区
	 */
	char read_buffer_[1024];
	char write_buffer_[1024];
};
}

#endif
