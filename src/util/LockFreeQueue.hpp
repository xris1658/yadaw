#ifndef YADAW_SRC_UTIL_LOCKFREEQUEUE
#define YADAW_SRC_UTIL_LOCKFREEQUEUE

#include "util/CompilerSpecifics.hpp"

#include <atomic>

namespace YADAW::Util
{
// Single producer + single consumer
template<typename T>
class LockFreeQueue
{
    using Self = LockFreeQueue<T>;
public:
    LockFreeQueue(std::size_t capacity):
    data_(reinterpret_cast<T*>(std::malloc(sizeof(T) * (capacity + 1)))),
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
        std::free(data_);
    }
public:
    std::size_t size() const noexcept
    {
        return end_.load(std::memory_order::memory_order_acquire) - begin_.load(std::memory_order::memory_order_acquire);
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
        end_.fetch_add(1, std::memory_order::memory_order_acq_rel);
    }
    ALWAYS_INLINE void bumpUpBegin()
    {
        begin_.fetch_add(1, std::memory_order::memory_order_acq_rel);
    }
public:
    bool push(const T& obj)
    {
        if(!full())
        {
            auto dest = end_.load(std::memory_order::memory_order_acquire);
            if(dest == capacity_ + 1)
            {
                dest = 0;
            }
            new (data_ + dest) T(obj);
            return true;
        }
        return false;
    }
    bool push(T&& obj)
    {
        if(!full())
        {
            auto dest = end_.load(std::memory_order::memory_order_acquire);
            if(dest == capacity_ + 1)
            {
                dest = 0;
            }
            new (data_ + dest) T(std::move(obj));
            return true;
        }
        return false;
    }
    template<typename... Args>
    bool emplace(Args&&... args)
    {

        if(!full())
        {
            auto dest = end_.load(std::memory_order::memory_order_acquire);
            if(dest == capacity_ + 1)
            {
                dest = 0;
            }
            new (data_ + dest) T(std::forward<Args>(args)...);
            return true;
        }
        return false;
    }
    bool popTo(T& obj)
    {
        if(!empty())
        {
            auto ptr = data_ + (begin_.load(std::memory_order::memory_order_acquire) % (capacity_ + 1));
            if(std::is_move_assignable_v<T>)
            {
                obj = std::move(*ptr);
            }
            else
            {
                obj = ptr;
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
            auto ptr = data_ + (begin_.load(std::memory_order::memory_order_acquire) % (capacity_ + 1));
            if(!std::is_trivially_destructible_v<T>)
            {
                ptr->~T();
            }
            bumpUpBegin();
            return true;
        }
    }
private:
    T* data_;
    std::size_t capacity_;
    std::atomic_size_t begin_ = 0;
    std::atomic_size_t end_ = 0;
};
}

#endif //YADAW_SRC_UTIL_LOCKFREEQUEUE
