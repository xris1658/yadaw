#include "AtomicMutex.hpp"

namespace YADAW::Util
{
AtomicMutex::LockGuard::LockGuard(AtomicMutex& lock):
    value_(lock.value_)
{
    while(value_.load(std::memory_order::memory_order_acquire)) {}
    value_.store(true, std::memory_order::memory_order_release);
}

AtomicMutex::LockGuard::~LockGuard()
{
    value_.store(false, std::memory_order::memory_order_release);
}
}