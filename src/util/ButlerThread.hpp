#ifndef YADAW_SRC_UTIL_BUTLERTHREAD
#define YADAW_SRC_UTIL_BUTLERTHREAD

#include "util/CircularDeque.hpp"
#include "util/LockFreeQueue.hpp"

#include <functional>

namespace YADAW::Util
{
class ButlerThread
{
public:
    ButlerThread(std::size_t taskCapacity);
    ~ButlerThread();
public:
    bool addTask(const std::function<void()>& task);
    bool addTask(std::function<void()>&& task);
    template<typename... Args>
    bool emplaceTask(Args&&... args)
    {
        std::unique_lock<std::mutex> lg(mutex_);
        auto ret = tasks_.emplace(std::forward<Args>(args)...);
        if(ret)
        {
            cv_.notify_one();
        }
        return ret;
    }
private:
    void butlerThreadFunc();
private:
    YADAW::Util::LockFreeQueue<std::function<void()>> tasks_;
    std::thread thread_;
    bool run_;
    std::mutex mutex_;
    std::condition_variable_any cv_;
};
}

#endif // YADAW_SRC_UTIL_BUTLERTHREAD
