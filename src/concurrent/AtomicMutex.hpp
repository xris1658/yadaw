#ifndef YADAW_SRC_CONCURRENT_ATOMICMUTEX
#define YADAW_SRC_CONCURRENT_ATOMICMUTEX

#include <atomic>

namespace YADAW::Concurrent
{
class AtomicMutex
{
public:
    AtomicMutex() = default;
    AtomicMutex(const AtomicMutex&) = delete;
    AtomicMutex(AtomicMutex&&) = delete;
    ~AtomicMutex() noexcept = default;
public:
    void lock();
    bool try_lock();
    void unlock();
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};
}

#endif // YADAW_SRC_CONCURRENT_ATOMICMUTEX
