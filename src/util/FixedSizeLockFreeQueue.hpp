#ifndef YADAW_SRC_UTIL_FIXEDSIZELOCKFREEQUEUE
#define YADAW_SRC_UTIL_FIXEDSIZELOCKFREEQUEUE

#include "util/CompilerSpecifics.hpp"

#include <array>
#include <atomic>

namespace YADAW::Util
{
// Single producer + single consumer
template<typename T, std::size_t Capacity>
class FixedSizeLockFreeQueue
{
    using Self = FixedSizeLockFreeQueue<T, Capacity>;
public:
    FixedSizeLockFreeQueue() {}
    FixedSizeLockFreeQueue(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    FixedSizeLockFreeQueue(Self&&) = delete;
    Self& operator=(Self&&) = delete;
    ~FixedSizeLockFreeQueue()
    {
        while(pop()) {}
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
    static constexpr std::size_t capacity() noexcept
    {
        return Capacity - 1;
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
            // 1. Construct at tail
            auto dest = end_.load(std::memory_order::memory_order_acquire) % (Capacity + 1);
            new(data_.data() + dest) T(obj);
            // 2. Incrument tail
            bumpUpEnd();
            return true;
        }
        return false;
    }
    bool push(T&& obj)
    {
        if(!full())
        {
            // 1. Construct at tail
            auto dest = end_.load(std::memory_order::memory_order_acquire) % (Capacity + 1);
            new(data_.data() + dest) T(std::move(obj));
            // 2. Incrument tail
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
            // 1. Construct at tail
            auto dest = end_.load(std::memory_order::memory_order_acquire) % (Capacity + 1);
            new(data_.data() + dest) T(std::forward<Args>(args)...);
            // 2. Incrument tail
            bumpUpEnd();
            return true;
        }
        return false;
    }
    bool popTo(T& obj)
    {
        if(!empty())
        {
            // 1. Move and destruct at head
            auto ptr = reinterpret_cast<T*>(data_.data()) +
                (begin_.load(std::memory_order::memory_order_acquire) % (Capacity + 1));
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
            // 2. Increment head
            bumpUpBegin();
            return true;
        }
        return false;
    }
    bool pop()
    {
        if(!empty())
        {
            // 1. Move and destruct at head
            auto ptr = reinterpret_cast<T*>(data_.data()) +
                (begin_.load(std::memory_order::memory_order_acquire) % (Capacity + 1));
            if(!std::is_trivially_destructible_v<T>)
            {
                ptr->~T();
            }
            // 2. Increment head
            bumpUpBegin();
            return true;
        }
        return false;
    }
private:
    std::array<std::byte[sizeof(T)], Capacity + 1> data_;
    std::atomic_size_t begin_ = 0;
    std::atomic_size_t end_ = 0;
};
}

#endif //YADAW_SRC_UTIL_FIXEDSIZELOCKFREEQUEUE
