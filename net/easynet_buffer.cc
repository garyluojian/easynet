#include "easynet_buffer.h"
#include <string.h>
#include <sys/uio.h>
#include <vector>
#include <glog/logging.h>

namespace easynet
{
    namespace net
    {
        Buffer::Buffer(size_t chunk_size)
        {
            _chunk_size = chunk_size;
            _buffer_size = chunk_size;
            _inuse_buffer_size = 0;
            char *chunk_buffer = new char[chunk_size];

            _buffer.push_back(chunk_buffer);
            _read_ptr.chunk_idx = _buffer.begin();
            _read_ptr.offset = 0;
            _write_ptr.chunk_idx = _buffer.begin();
            _write_ptr.offset = 0;
        }
        Buffer::~Buffer()
        {
            free();
        }

        void Buffer::free()
        {
            for(auto &it : _buffer)
            {
                delete [] it;
            }
            for(auto &it : _free_buffer)
            {
                delete [] it;
            }
        }

        void Buffer::writeData(const void *data, size_t data_size)
        {
            size_t total = data_size;
            size_t offset = 0;
            const char *c_data = static_cast<const char*>(data);
            expand(data_size); // ensure the space is enough

            do
            {
                size_t free_size_of_this_chunk = _chunk_size - _write_ptr.offset;
                if(data_size >= free_size_of_this_chunk)
                {
                    memcpy(&(*_write_ptr.chunk_idx)[_write_ptr.offset],
                        c_data + offset, free_size_of_this_chunk);
                    offset += free_size_of_this_chunk;
                    data_size -= free_size_of_this_chunk;
                    _write_ptr.chunk_idx++;
                    _write_ptr.offset = 0;
                }
                else
                {
                    memcpy(&(*_write_ptr.chunk_idx)[_write_ptr.offset],
                        c_data + offset, data_size);
                    _write_ptr.offset += data_size;
                    data_size = 0;
                }
            }while(data_size);
            _inuse_buffer_size += total;
        }

        int Buffer::readData(void *data, size_t data_size)
        {
            size_t offset = 0;
            char *c_data = static_cast<char*>(data);

            if(_inuse_buffer_size < data_size)
            {
                // There is no enough data
                return -1;
            }
            _inuse_buffer_size -= data_size;
            do
            {
                size_t used_size_of_this_chunk = _chunk_size - _read_ptr.offset;
                if(used_size_of_this_chunk > data_size)
                {
                    memcpy(c_data + offset, &(*_read_ptr.chunk_idx)[_read_ptr.offset],
                        data_size);
                    _read_ptr.offset += data_size;
                    data_size = 0;
                }
                else
                {
                    memcpy(c_data + offset, &(*_read_ptr.chunk_idx)[_read_ptr.offset],
                        used_size_of_this_chunk);
                    offset += used_size_of_this_chunk;
                    data_size -= used_size_of_this_chunk;
                    auto tmp = _read_ptr.chunk_idx++;
                    _read_ptr.offset = 0;
                    char *free_chunk = *tmp;
                    _buffer.erase(tmp);
                    _free_buffer.push_back(free_chunk);
                    _buffer_size -= _chunk_size;
                }
            }while(data_size);

            return 0;
        }

        void Buffer::expand(size_t at_least_size)
        {
            if(_buffer_size - _inuse_buffer_size > at_least_size)
            {
                return;
            }
            char *chunk;
            if(_free_buffer.empty())
            {
                chunk = new char[_chunk_size];
            }
            else
            {
                chunk = _free_buffer.front();
                _free_buffer.pop_front();
            }
            _buffer.push_back(chunk);
            _buffer_size += _chunk_size;
        }

        void Buffer::write(int fd)
        {
            std::vector<struct iovec> iovs;
            ssize_t n = 0;
            size_t i = 0;
            bool done = false;

            if(empty()) return;
            iovs.reserve((_inuse_buffer_size + _chunk_size - 1) / _chunk_size);

            chunk_ptr ptr = _read_ptr;
            do
            {
                struct iovec iov = {(*ptr.chunk_idx) + ptr.offset, _chunk_size - ptr.offset};

                if(ptr.chunk_idx == _write_ptr.chunk_idx)
                {
                    iov.iov_base = *ptr.chunk_idx + ptr.offset;
                    iov.iov_len = _write_ptr.offset - ptr.offset;
                    done = true;
                }
                else
                {
                    iov.iov_base = *ptr.chunk_idx + ptr.offset;
                    iov.iov_len = _chunk_size - ptr.offset;
                }
                iovs.push_back(iov);
                ptr.chunk_idx++;
                ptr.offset = 0;
            }while(!done);
            n = ::writev(fd, &(*iovs.begin()), iovs.size());
            if(n < 0)
            {
                LOG_EVERY_N(WARNING, 10)<<"Write data to "<<fd<<" failed.";
                return;
            }

            commitWrite(static_cast<size_t >(n));
        }

        void Buffer::commitWrite(size_t n)
        {
            size_t remain = n;

            CHECK(_inuse_buffer_size >= n)<<"inuse_buffer_size: "<<_inuse_buffer_size<<" n: "<<n;
            while(remain > 0)
            {
                if(_chunk_size - _read_ptr.offset >= n)
                {
                    _read_ptr.offset += n;
                    remain = 0;
                }
                else
                {
                    auto tmp = _read_ptr.chunk_idx++;
                    _read_ptr.offset = 0;
                    char *free_chunk = *tmp;
                    _buffer.erase(tmp);
                    _free_buffer.push_back(free_chunk);
                    remain -= _chunk_size;
                }
            }
            _inuse_buffer_size -= n;
        }
    }
}
