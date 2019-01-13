#ifndef __EASYNET_ENDIAN_H__
#define __EASYNET_ENDIAN_H__

#include <stdint.h>

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1
#endif

namespace easynet
{
    namespace sys
    {
        inline int64_t littleEndian2Big(int64_t little)
        {
            return ((little << 56) &    0xff00000000000000)
                   | ((little << 40) &  0xff000000000000)
                   | ((little << 24) &  0xff0000000000)
                   | ((little << 8) &   0xff00000000)
                   | ((little >> 8) &   0xff000000)
                   | ((little >> 24) &  0xff0000)
                   | ((little >> 40) &  0xff00)
                   | ((little >> 56) &  0xff);
        }

        inline uint64_t littleEndian2Big(uint64_t little)
        {
            return ((little << 56) &    0xff00000000000000)
                   | ((little << 40) &  0xff000000000000)
                   | ((little << 24) &  0xff0000000000)
                   | ((little << 8) &   0xff00000000)
                   | ((little >> 8) &   0xff000000)
                   | ((little >> 24) &  0xff0000)
                   | ((little >> 40) &  0xff00)
                   | ((little >> 56) &  0xff);
        }
        inline int32_t littleEndian2Big(int32_t little)
        {
            return (((little << 24) & 0xff000000)
                | ((little << 8) & 0xff0000)
                | ((little >> 8) & 0xff00)
                | ((little >> 24) & 0xff));
        }

        inline uint32_t littleEndian2Big(uint32_t little)
        {
            return (((little << 24) & 0xff000000)
                    | ((little << 8) & 0xff0000)
                    | ((little >> 8) & 0xff00)
                    | ((little >> 24) & 0xff));
        }

        inline int16_t littleEndian2Big(int16_t little)
        {
            return ((little << 8) & 0xff00)
                | ((little >> 8) & 0xff);
        }

        inline uint16_t littleEndian2Big(uint16_t little)
        {
            return ((little >> 8) & 0xff)
                   | ((little << 8) & 0xff00);
        }

        inline int8_t littleEndian2Big(int8_t little)
        {
            return little;
        }

        inline uint8_t littleEndian2Big(uint8_t little)
        {
            return little;
        }

        template<typename T> inline T bigEndian2Little(T t)
        {
            return littleEndian2Big(t);
        }

        template<typename T> inline T host2Net(T t)
        {
#ifdef LITTLE_ENDIAN
            return littleEndian2Big(t);
#else
            return t;
#endif
        }

        template<typename T> inline T net2Host(T t)
        {
#ifdef LITTLE_ENDIAN
            return bigEndian2Little(t);
#else
            return t;
#endif
        }
    }
}

#endif