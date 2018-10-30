#ifndef __EASYNET_CONDITION_H__
#define __EASYNET_CONDITION_H__

#include <pthread.h>
#include <stdint.h>

namespace easynet
{
    namespace sys
    {
        class easynet_mutex;
        class easynet_condition
        {
        public:
            explicit easynet_condition(easynet_mutex &mutex)
                : _mutex_handle(mutex)
            {
                int rc = pthread_cond_init(&_native_cond_handle, NULL);
            }
            ~easynet_condition()
            {
                int rc = pthread_cond_destroy(&_native_cond_handle);
            }

            void wait();
            bool time_wait(uint64_t timeout);
            void notify();
            void notify_all();
        private:
            easynet_mutex &_mutex_handle;
            pthread_cond_t _native_cond_handle;
        };
    }
}

#endif