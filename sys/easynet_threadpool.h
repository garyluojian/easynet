#ifndef __EASYNET_THREADPOOL_H__
#define __EASYNET_THREADPOOL_H__

#include <vector>
#include <memory>
#include "easynet_thread.h"
#include "easynet_mutex.h"
#include "easynet_condition.h"

namespace easynet
{
    namespace sys
    {
        class easynet_threadpool
        {
        public:
            easynet_threadpool();
            explicit easynet_threadpool(int thread_num);
            ~easynet_threadpool();

            void start();
            void stop();

            void run();
        private:
            easynet_mutex _mutex;
            easynet_condition _condition;
            int _thread_num;
            std::vector<std::shared_ptr<easynet_thread>> _thread_pool;
        };
    }
}

#endif