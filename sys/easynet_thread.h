#ifndef __EASYNET_THREAD_H_
#define __EASYNET_THREAD_H_

#include <pthread.h>
#include <functional>
#include <string>

namespace easynet
{
namespace sys
{
class easynet_thread
{
public:
    using thread_func = std::function<void(void*)>;
    explicit easynet_thread(thread_func callback, void *data, const std::string &thread_num);
    ~easynet_thread();

    void start();
    void join();

private:
    static void *hook(void *args);
private:
    pthread_t _thread_id;
    thread_func _callback;
    void *_callback_ctx;
    std::string _thread_name;
    bool _started;
};
}
}

#endif