#ifndef __EASYNET_BUFFER_H__
#define __EASYNET_BUFFER_H__

#include <stdlib.h>
#include <list>
#include <cstddef>

/*
    data chunk --> data chunk --> data chunk
    free chunk --> free_chunk --> free_chunk
*/
namespace easynet 
{
    namespace net
    {
        class Buffer
        {
            enum { DEFAULT_BUFFER_CHUNK_SIZE = 1<<20};
            public:
                explicit  Buffer(size_t chunk_size = DEFAULT_BUFFER_CHUNK_SIZE);
                ~Buffer();

                void writeData(const void *data, size_t data_size);
                int readData(void *data, size_t data_size);
                bool empty() {return _inuse_buffer_size == 0;}
                size_t size() {return _inuse_buffer_size;}
                void write(int fd);

            private:
                void expand(size_t at_least_size);
                void free();
                void commitWrite(size_t n);
            private:
                size_t _chunk_size;
                size_t _buffer_size;
                size_t _inuse_buffer_size;
                std::list<char*> _buffer;
                std::list<char*> _free_buffer;
                struct chunk_ptr
                {
                    std::list<char*>::iterator chunk_idx;
                    size_t offset;
                };
                chunk_ptr _read_ptr;
                chunk_ptr _write_ptr;
        };
    }
}


#endif
