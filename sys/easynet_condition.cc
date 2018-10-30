#include "easynet_condition.h"
#include "easynet_mutex.h"
#include <time.h>
#include <errno.h>

namespace easynet
{
    namespace sys
    {
        void easynet_condition::wait()
        {
            int rc = pthread_cond_wait(&_native_cond_handle,
                _mutex_handle.get_mutex_native_handle());
        }

        void easynet_condition::notify()
        {
            int rc = pthread_cond_signal(&_native_cond_handle);
        }

        void easynet_condition::notify_all()
        {
            int rc = pthread_cond_broadcast(&_native_cond_handle);
        }

        bool easynet_condition::time_wait(uint64_t timeout)
        {
            struct timespec ts;

            clock_gettime(CLOCK_REALTIME, &ts);

            uint64_t sec = timeout / 1000;
            uint64_t nsec = (timeout % 1000) * 1e6;

            ts.tv_sec += static_cast<time_t>(sec);
            ts.tv_nsec += static_cast<long>(nsec);

            int rc = pthread_cond_timedwait(&_native_cond_handle, 
                    _mutex_handle.get_mutex_native_handle(), &ts);

            return rc == ETIMEDOUT ? true : false;
        }
    }
}