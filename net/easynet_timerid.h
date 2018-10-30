#ifndef __EASYNET_TIMERID_H__
#define __EASYNET_TIMERID_H__

#include <atomic>
#include <cstdint>

namespace easynet
{
    namespace net
    {
        struct TimerId {
        private:
            class TimerIdGenerator {
            public:
                static uint64_t get() {
                    return _id_genetator.fetch_add(1);
                }

            private:
                static std::atomic_uint64_t _id_genetator;
            };

        public:
            TimerId() {
                _id = TimerIdGenerator::get();
            }

            uint64_t _id;
        };

        inline bool operator==(const TimerId &l, const TimerId &r)
        {
            return l._id == r._id;
        }

        inline bool operator<(const TimerId &l, const TimerId &r)
        {
            return l._id < r._id;
        }
    }
}

#endif