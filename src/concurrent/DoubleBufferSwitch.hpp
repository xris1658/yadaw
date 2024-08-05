#ifndef YADAW_SRC_CONCURRENT_DOUBLEBUFFERSWITCH
#define YADAW_SRC_CONCURRENT_DOUBLEBUFFERSWITCH

#include <atomic>

namespace YADAW::Concurrent
{
template<typename T>
class DoubleBufferSwitch
{
public:
    DoubleBufferSwitch() {}
    DoubleBufferSwitch(const DoubleBufferSwitch<T>&) = delete;
    ~DoubleBufferSwitch() {}
public:
    T get() const noexcept
    {
        return value_.load(std::memory_order_acquire);
    }
    T flip() noexcept
    {
        return value_.fetch_xor(1, std::memory_order_acq_rel);
    }
private:
    std::atomic<T> value_;
};
}

#endif // YADAW_SRC_CONCURRENT_DOUBLEBUFFERSWITCH
