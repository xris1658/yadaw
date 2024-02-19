#ifndef YADAW_SRC_CONCURRENT_PASSDATATOREALTIMETHREAD
#define YADAW_SRC_CONCURRENT_PASSDATATOREALTIMETHREAD

#include <array>
#include <atomic>
#include <memory>

namespace YADAW::Concurrent
{
template<typename T>
class PassDataToRealtimeThread
{
public:
    PassDataToRealtimeThread(std::unique_ptr<T>&& initialData):
        data_{std::move(initialData), std::unique_ptr<T>(nullptr)},
        updated_(false)
    {}
public:
    T* get()
    {
        return data_[0].get();
    }
    const T* get() const
    {
        return data_[0].get();
    }
    void update(std::unique_ptr<T>&& data, bool realtimeThreadRunning = true)
    {
        if(realtimeThreadRunning)
        {
            while(updated_.load(std::memory_order::memory_order_acquire)) {}
        }
        while(updated_.load(std::memory_order::memory_order_acquire)) {}
        data_[1] = std::move(data);
        updated_.store(true, std::memory_order::memory_order_release);
    }
    void swapIfNeeded()
    {
        if(updated_.load(std::memory_order::memory_order_acquire))
        {
            std::swap(data_[0], data_[1]);
            updated_.store(false, std::memory_order::memory_order_release);
        }
    }
    void disposeOld(bool realtimeThreadRunning = true)
    {
        if(realtimeThreadRunning)
        {
            while(updated_.load(std::memory_order::memory_order_acquire)) {}
        }
        data_[1].reset();
    }
    void updateAndDispose(std::unique_ptr<T>&& data, bool realtimeThreadRunning = true)
    {
        if(realtimeThreadRunning)
        {
            update(std::move(data), realtimeThreadRunning);
            disposeOld(realtimeThreadRunning);
        }
        else
        {
            data_[0] = std::move(data); // ABA happens here, but maybe it's okay?
        }
    }
private:
    std::array<std::unique_ptr<T>, 2> data_;
    std::atomic<bool> updated_;
};
}

#endif // YADAW_SRC_CONCURRENT_PASSDATATOREALTIMETHREAD
