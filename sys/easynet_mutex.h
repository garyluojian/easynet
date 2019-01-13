#ifndef __EASYNET_MUTEX_H__
#define __EASYNET_MUTEX_H__

#include <pthread.h>
#include <glog/logging.h>

namespace easynet
{
    namespace sys
    {
        class easynet_mutex
        {
        public:
            easynet_mutex()
            {
                int rc = pthread_mutex_init(&_native_mutex_handle, NULL);
                LOG_IF(FATAL, rc != 0)<<"pthread_mutex_init failed, rc = "<<rc;
            }
            ~easynet_mutex()
            {
                int rc = pthread_mutex_destroy(&_native_mutex_handle);
                LOG_IF(FATAL, rc != 0)<<"pthread_mutex_destroy failed, rc = "<<rc;
            }

            void lock()
            {
                int rc = pthread_mutex_lock(&_native_mutex_handle);
                LOG_IF(FATAL, rc != 0)<<"pthread_mutex_lock failed, rc = "<<rc;
            }
            void unlock()
            {
                int rc = pthread_mutex_unlock(&_native_mutex_handle);
                LOG_IF(FATAL, rc != 0)<<"pthread_mutex_unlock failed, r = "<<rc;
            }
            pthread_mutex_t *get_mutex_native_handle()
            {
                return &_native_mutex_handle;
            }

        private:
            pthread_mutex_t _native_mutex_handle;
        };

        class easynet_scoped_mutex
        {
        public:
            explicit easynet_scoped_mutex(easynet_mutex &mutex)
                : _mutex(&mutex)
            {
                _mutex->lock();
            }
            ~easynet_scoped_mutex()
            {
                _mutex->unlock();
            }

        private:
            easynet_mutex *_mutex;
        };
    }
}

#endif