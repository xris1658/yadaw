#ifndef YADAW_SRC_UTIL_ATOMICMUTEX
#define YADAW_SRC_UTIL_ATOMICMUTEX

#include <atomic>

namespace YADAW::Util
{
class AtomicMutex
{
public:
    class LockGuard
    {
    public:
        LockGuard(AtomicMutex& lock);
        LockGuard(const LockGuard&) = delete;
        LockGuard(LockGuard&&) = delete;
        ~LockGuard();
    private:
        std::atomic_flag& flag_;
    };
    friend LockGuard;
public:
    AtomicMutex() = default;
    AtomicMutex(const AtomicMutex&) = delete;
    AtomicMutex(AtomicMutex&&) = delete;
    ~AtomicMutex() noexcept = default;
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

using AtomicLockGuard = AtomicMutex::LockGuard;
}

#endif // YADAW_SRC_UTIL_ATOMICMUTEX
