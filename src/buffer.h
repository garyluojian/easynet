#ifndef _EASYNET_BUFFER_H_
#define _EASYNET_BUFFER_H_

#include <memory>
#include <stdint.h>
#include <string>

class Buffer
{
public:
    Buffer();
    Buffer(int size);
    ~Buffer();

    void resize(int size);
    void reset();
    int write_u8(uint8_t u8)
    {
        return append(&u8, sizeof(u8));
    }
    int write_u16(uint16_t u16)
    {
        uint16_t x = u16_host_to_net(u16);

        return append(&x, sizeof(x));
    }
    int write_u32(uint32_t u32)
    {
        uint32_t x = u32_host_to_net(u32);
        return append(&x, sizeof(x));
    }
    int write_u64(uint64_t u64)
    {
        uint64_t x = u64_host_to_net(u64);
        return append(&x, sizeof(x));
    }
    int write_binary(const void *b, size_t size)
    {
        return append(b, size);
    }
    int write_string(const std::string &str)
    {
        return write_string(str.c_str(), str.size());
    }
    int write_string(const char *str, size_t size)
    {
        return write_binary(str, size);
    }

    int read_u8(uint8_t *u8)
    {
        return pop(u8, sizeof(*u8));
    }
    int read_u16(uint16_t *u16)
    {
        uint16_t n;
        int rc;

        rc = pop(&n, sizeof(n));
        *u16 = u16_net_to_host(n);

        return rc;
    }
    int read_u32(uint32_t *u32)
    {
        uint32_t n;
        int rc;

        rc = pop(&n, sizeof(n));
        *u32 = u32_net_to_host(n);

        return rc;
    }
    int read_u64(uint64_t *u64)
    {
        uint64_t n;
        int rc;

        rc = pop(&n, sizeof(n));
        *u64 = u64_net_to_host(n);

        return rc;
    }
    int read_binary(void *b, size_t size)
    {
        return pop(b, size);
    }
    int read_string(std::string &str, size_t size)
    {
         
    }
    int read_string(char *str, size_t size)
    {
        return read_binary(str, size);
    }

private:
    int append(const void *d, size_t size);
    int pop(void *d, size_t size);
    uint16_t u16_host_to_net(uint16_t u16);
    uint16_t u16_net_to_host(uint16_t u16);
    uint32_t u32_host_to_net(uint32_t u32);
    uint32_t u32_net_to_host(uint32_t u32);
    uint64_t u64_host_to_net(uint64_t u64);
    uint64_t u64_net_to_host(uint64_t u64);
    
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

private:
    std::unique_ptr<char> buffer_;
    int capacity_;
    int size_;
    int read_ptr_;
    int write_ptr_;
};

#endif /*_EASYNET_BUFFER_H_*/