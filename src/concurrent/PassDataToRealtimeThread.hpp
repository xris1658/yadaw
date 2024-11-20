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
    PassDataToRealtimeThread(const T& initialData):
        data_{initialData, T()},
        updated_(false)
    {}
    PassDataToRealtimeThread(T&& initialData):
        data_{std::move(initialData), T()},
        updated_(false)
    {}
    template<typename... Args>
    PassDataToRealtimeThread(Args&&... args):
        data_{T(std::forward<Args>(args)...), T()},
        updated_(false)
    {}
public:
    T& get()
    {
        return data_[0];
    }
    const T& get() const
    {
        return data_[0];
    }
    void update(const T& data, bool realtimeThreadRunning = true)
    {
        if(realtimeThreadRunning)
        {
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        data_[1] = data;
        updated_.store(true, std::memory_order_release);
    }
    void update(T&& data, bool realtimeThreadRunning =
#if __APPLE__
        false
#else
        true
#endif
    )
    {
        if(realtimeThreadRunning)
        {
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        data_[1] = std::move(data);
        updated_.store(true, std::memory_order_release);
    }
    template<typename... Args>
    void emplace(Args&&... args, bool realtimeThreadRunning =
#if __APPLE__
        false
#else
        true
#endif
        )
    {
        if(realtimeThreadRunning)
        {
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        data_[1] = T(std::forward<Args>(args)...);
        updated_.store(true, std::memory_order_release);
    }
    void swapIfNeeded()
    {
        if(updated_.load(std::memory_order_acquire))
        {
            std::swap(data_[0], data_[1]);
            updated_.store(false, std::memory_order_release);
#if __cplusplus >= 202002L
            updated_.notify_one();
#endif
        }
    }
    void disposeOld(bool realtimeThreadRunning = true)
    {
        if(realtimeThreadRunning)
        {
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
    }
    void updateAndDispose(const T& data, bool realtimeThreadRunning =
#if __APPLE__
        false
#else
        true
#endif
        )
    {
        if(realtimeThreadRunning)
        {
            update(data, realtimeThreadRunning);
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        else
        {
            data_[0] = data; // ABA happens here, but maybe it's okay?
        }
    }
    void updateAndDispose(T&& data, bool realtimeThreadRunning =
#if __APPLE__
        false
#else
        true
#endif
        )
    {
        if(realtimeThreadRunning)
        {
            update(std::move(data), realtimeThreadRunning);
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        else
        {
            data_[0] = std::move(data); // ABA happens here, but maybe it's okay?
        }
    }
    template<typename... Args>
    void emplaceAndDispose(Args&&... args, bool realtimeThreadRunning =
#if __APPLE__
        false
#else
        true
#endif
        )
    {
        if(realtimeThreadRunning)
        {
            update(std::forward<Args>(args)..., realtimeThreadRunning);
#if __cplusplus >= 202002L
            updated_.wait(true, std::memory_order_acquire);
#else
            while(updated_.load(std::memory_order_acquire)) {}
#endif
        }
        else
        {
            data_[0] = T(std::forward<Args>(args)...); // ABA happens here, but maybe it's okay?
        }
    }
private:
    std::array<T, 2> data_;
    std::atomic<bool> updated_;
};
}

#endif // YADAW_SRC_CONCURRENT_PASSDATATOREALTIMETHREAD
