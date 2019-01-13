#include "easynet_thread.h"
#include <glog/logging.h>
#include <cassert>

namespace easynet
{
    namespace sys
    {
        easynet_thread::easynet_thread(
            thread_func callback, 
            void *data,
            const std::string &thread_name)
            : _callback(callback),
            _callback_ctx(data),
            _thread_name(thread_name),
            _started(false)
        {            
        }

        easynet_thread::~easynet_thread(){}

        void easynet_thread::start()
        {
            assert(!_started);
            _started = true;
            int rc = pthread_create(&_thread_id, NULL,
                    &easynet_thread::hook, this);
            LOG_IF(FATAL, rc != 0)<<"Create pthread fail, rc = "<<rc;
        }

        void easynet_thread::join()
        {
            int rc = pthread_join(_thread_id, NULL);
            LOG_IF(FATAL, rc != 0)<<"Join thread fail, rc = "<<rc;
        }

        void *easynet_thread::hook(void *args)
        {
            easynet_thread *myself = static_cast<easynet_thread*>(args);
            
            myself->_callback(myself->_callback_ctx);

            return NULL;
        }
    }
}