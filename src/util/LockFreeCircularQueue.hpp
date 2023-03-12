#ifndef YADAW_SRC_UTIL_LOCKFREECIRCULARQUEUE
#define YADAW_SRC_UTIL_LOCKFREECIRCULARQUEUE

#include "util/CompilerSpecifics.hpp"

#include <array>
#include <atomic>

namespace YADAW::Util
{
template<typename T, std::size_t Capacity>
class LockFreeCircularQueue
{
    using Self = LockFreeCircularQueue<T, Capacity>;
public:
    LockFreeCircularQueue() {}
    LockFreeCircularQueue(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    LockFreeCircularQueue(Self&&) = delete;
    Self& operator=(Self&&) = delete;
    ~LockFreeCircularQueue()
    {
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
    ALWAYS_INLINE std::size_t bumpUpEnd()
    {
        return end_.fetch_add(1, std::memory_order::memory_order_acq_rel);
    }
    ALWAYS_INLINE std::size_t bumpUpBegin()
    {
        return begin_.fetch_add(1, std::memory_order::memory_order_acq_rel);
    }
public:
    bool push(const T& obj)
    {
        if(!full())
        {
            auto dest = bumpUpEnd();
            if(dest == Capacity)
            {
                dest = 0;
            }
            new(data_.data() + dest) T(obj);
            return true;
        }
        return false;
    }
    bool push(T&& obj)
    {
        if(!full())
        {
            auto dest = bumpUpEnd();
            if(dest == Capacity)
            {
                dest = 0;
            }
            new(data_.data() + dest) T(std::move(obj));
            return true;
        }
        return false;
    }
    template<typename... Args>
    bool emplace(Args&&... args)
    {
        if(!full())
        {
            auto dest = bumpUpEnd();
            if(dest == Capacity)
            {
                dest = 0;
            }
            new(data_.data() + dest) T(std::forward<Args>(args)...);
            return true;
        }
        return false;
    }
    bool popTo(T& obj)
    {
        // TODO
    }
private:
    std::array<std::byte[sizeof(T)], Capacity> data_;
    std::atomic_size_t begin_ = 0;
    std::atomic_size_t end_ = 0;
};
}

#endif //YADAW_SRC_UTIL_LOCKFREECIRCULARQUEUE
