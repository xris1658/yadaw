#ifndef YADAW_SRC_UTIL_LOCKFREEQUEUE
#define YADAW_SRC_UTIL_LOCKFREEQUEUE

#include "util/AlignHelper.hpp"
#include "util/CompilerSpecifics.hpp"

#include <atomic>
#include <cstdlib>
#include <new>

namespace YADAW::Util
{
#ifdef __cpp_lib_hardware_interference_size
using std::hardware_destructive_interference_size;
using std::hardware_constructive_interference_size;
#else
inline const std::size_t hardware_destructive_interference_size = 64;
inline const std::size_t hardware_constructive_interference_size = 64;
#endif
// Single producer + single consumer
template<typename T>
class LockFreeQueue: AlignHelper<T>
{
    using Self = LockFreeQueue<T>;
    using Aligned = typename AlignHelper<T>::Aligned;
public:
    LockFreeQueue(std::size_t capacity):
    data_(new(std::nothrow) Aligned[capacity + 1]),
    capacity_(capacity)
    {
        if(!data_)
        {
            capacity_ = 0;
        }
    }
    LockFreeQueue(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    LockFreeQueue(Self&&) = delete;
    Self& operator=(Self&&) = delete;
    ~LockFreeQueue()
    {
        while(pop()) {}
        delete[] data_;
    }
public:
    std::size_t size() const noexcept
    {
        return end_.load(std::memory_order_acquire) - begin_.load(std::memory_order_acquire);
    }
    std::size_t empty() const noexcept
    {
        return size() == 0;
    }
    std::size_t full() const noexcept
    {
        return size() == capacity();
    }
    std::size_t capacity() const noexcept
    {
        return capacity_;
    }
private:
    ALWAYS_INLINE void bumpUpEnd()
    {
        end_.fetch_add(1, std::memory_order_acq_rel);
    }
    ALWAYS_INLINE void bumpUpBegin()
    {
        begin_.fetch_add(1, std::memory_order_acq_rel);
    }
public:
    bool push(const T& obj)
    {
        if(!full())
        {
            auto dest = end_.load(std::memory_order_acquire) % (capacity_ + 1);
            new (data_ + dest) T(obj);
            bumpUpEnd();
            return true;
        }
        return false;
    }
    bool push(T&& obj)
    {
        if(!full())
        {
            auto dest = end_.load(std::memory_order_acquire) % (capacity_ + 1);
            new (data_ + dest) T(std::move(obj));
            bumpUpEnd();
            return true;
        }
        return false;
    }
    template<typename... Args>
    bool emplace(Args&&... args)
    {
        if(!full())
        {
            auto dest = end_.load(std::memory_order_acquire) % (capacity_ + 1);
            new (data_ + dest) T(std::forward<Args>(args)...);
            bumpUpEnd();
            return true;
        }
        return false;
    }
    bool popTo(T& obj)
    {
        if(!empty())
        {
            auto ptr = AlignHelper<T>::fromAligned(data_ + (begin_.load(std::memory_order_acquire) % (capacity_ + 1)));
            if(std::is_move_assignable_v<T>)
            {
                obj = std::move(*ptr);
            }
            else
            {
                obj = *ptr;
            }
            if(!std::is_trivially_destructible_v<T>)
            {
                ptr->~T();
            }
            bumpUpBegin();
            return true;
        }
        return false;
    }
    bool pop()
    {
        if(!empty())
        {
            auto ptr = AlignHelper<T>::fromAligned(data_ + (begin_.load(std::memory_order_acquire) % (capacity_ + 1)));
            if(!std::is_trivially_destructible_v<T>)
            {
                ptr->~T();
            }
            bumpUpBegin();
            return true;
        }
        return false;
    }
private:
    Aligned* data_;
    std::size_t capacity_;
    alignas(hardware_destructive_interference_size) std::atomic_size_t begin_{0};
    alignas(hardware_destructive_interference_size) std::atomic_size_t end_{0};
};
}

#endif // YADAW_SRC_UTIL_LOCKFREEQUEUE
