#ifndef __EASYNET_WEAKCALLBACK_H__
#define __EASYNET_WEAKCALLBACK_H__

#include <memory>
#include <functional>

namespace easynet
{
    namespace sys
    {
        template <typename T>
        class WeakCallback
        {
        public:
            WeakCallback(std::shared_ptr<T> &ptr,
                    const std::function<void(std::shared_ptr<T>)> &callback)
                    : _weak_ptr(ptr), _callback(callback)
            {}
            void operator()()
            {
                std::shared_ptr<T> ptr(_weak_ptr.lock());
                if(ptr)
                {
                    _callback(ptr);
                }
            }
        private:
            std::weak_ptr<T> _weak_ptr;
            std::function<void(std::shared_ptr<T>)> _callback;
        };
    }
}


#endif