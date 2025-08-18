#ifndef YADAW_SRC_CONCURRENT_FIXEDSIZELOCKFREEQUEUE
#define YADAW_SRC_CONCURRENT_FIXEDSIZELOCKFREEQUEUE

#include "util/AlignHelper.hpp"
#include "util/IntegerRange.hpp"
#include "util/OptionalUtil.hpp"

#include <array>
#include <atomic>
#include <memory>
#include <ranges>

namespace YADAW::Concurrent
{
// Single producer, single consumer queue
template<typename T, std::size_t Capacity>
class FixedSizeLockFreeQueue
{
    using AH = YADAW::Util::AlignHelper<T>;
public:
    class View
    {
        friend class FixedSizeLockFreeQueue;
    public:
        View(): pointer_(nullptr), head_(0), tail_(0) {}
    private:
        View(FixedSizeLockFreeQueue& queue, std::size_t head, std::size_t tail):
            pointer_(queue.container_.data()), head_(head), tail_(tail)
        {}
    public:
        View(const View& rhs) = default;
        ~View() = default;
    public:
        std::size_t size() const noexcept { return tail_ - head_; }
        T& operator[](std::size_t index) { return pointer_[(head_ + index) % (Capacity + 1)]; }
        const T& operator[](std::size_t index) const { return pointer_[(head_ + index) % (Capacity + 1)]; }
    private:
        YADAW::Util::AlignedStorage<T>* pointer_;
        std::size_t head_;
        std::size_t tail_;
    };
public:
    FixedSizeLockFreeQueue();
    ~FixedSizeLockFreeQueue()
    {
        clear();
    }
public:
    static constexpr std::size_t capacity() noexcept { return Capacity; }
    std::size_t size() const noexcept
    {
        auto ret = tail_.load(std::memory_order_relaxed) - head_.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        return ret;
    }
public:
    OptionalRef<T> back()
    {
        if(auto tail = tail_.load(std::memory_order_relaxed);
            tail > middle_.load(std::memory_order_relaxed))
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            return AH::fromAligned(container_.data() + tail);
        }
        return std::nullopt;
    }
    OptionalRef<T> pushBack(T& value)
    {
        if(auto tail = tail_.load(std::memory_order_relaxed);
            tail - head_.load(std::memory_order_relaxed) < capacity())
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            auto pointer = AH::fromAligned(container_.data() + tail);
            new(pointer) T(value);
            tail_.store(tail + 1, std::memory_order_release);
            return *pointer;
        }
        return std::nullopt;
    }
    OptionalRef<T> pushBack(T&& value)
    {

        if(auto tail = tail_.load(std::memory_order_relaxed);
            tail - head_.load(std::memory_order_relaxed) < capacity())
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            auto pointer = AH::fromAligned(container_.data() + tail);
            new(pointer) T(std::move(value));
            tail_.store(tail + 1, std::memory_order_release);
            return *pointer;
        }
        return std::nullopt;
    }
    template<typename... Args>
    OptionalRef<T> emplaceBack(Args&&... args)
    {
        if(auto tail = tail_.load(std::memory_order_relaxed);
            tail - head_.load(std::memory_order_relaxed) < capacity())
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            auto pointer = AH::fromAligned(container_.data() + tail);
            new(pointer) T(std::forward<Args>(args)...);
            tail_.store(tail + 1, std::memory_order_release);
            return *pointer;
        }
        return std::nullopt;
    }
public:
    void moveMiddleToTail()
    {
        middle_.store(tail_.load(std::memory_order_acquire), std::memory_order_release);
    }
    View createReadView()
    {
        auto head = head_.load(std::memory_order_relaxed);
        auto tail = tail_.load(std::memory_order_relaxed);
        std::size_t middle = head;
        auto noPointInThisProcess = middle_.compare_exchange_strong(
            middle, tail, std::memory_order_acq_rel, std::memory_order_acquire
        );
        return View(container_.data(), head, noPointInThisProcess? tail: middle);
    }
    void popToMiddle(const View& headToMiddleView)
    {
        if(!std::is_trivially_destructible_v<T>)
        {
            FOR_RANGE(i, headToMiddleView.head_, headToMiddleView.middle_)
            {
                std::destroy_at(AH::fromAligned(container_.data() + i % (Capacity + 1)));
            }
        }
        head_.store(headToMiddleView.middle_, std::memory_order_release);
    }
    void clear()
    {
        auto head = head_.load(std::memory_order_relaxed);
        auto tail = tail_.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if(head != tail)
        {
            if(!std::is_trivially_destructible_v<T>)
            {
                FOR_RANGE(i, head, tail)
                {
                    std::destroy_at(AH::fromAligned(container_.data() + i % (Capacity + 1)));
                }
            }
            std::atomic_thread_fence(std::memory_order_release);
            head_.store(0, std::memory_order_relaxed);
            middle_.store(0, std::memory_order_relaxed);
            tail_.store(0, std::memory_order_relaxed)
        }
    }
private:
    std::atomic_size_t head_{0};
    std::atomic_size_t middle_{0};
    std::atomic_size_t tail_{0};
    // TODO: Arrange values above in one or more cache lines depending on how
    //       often they're used in audio and UI threads
    std::array<YADAW::Util::AlignedStorage<T>, Capacity + 1> container_;
};
}

#endif // YADAW_SRC_CONCURRENT_FIXEDSIZELOCKFREEQUEUE