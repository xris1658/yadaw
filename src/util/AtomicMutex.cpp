#include "AtomicMutex.hpp"

namespace YADAW::Util
{
AtomicMutex::LockGuard::LockGuard(AtomicMutex& lock):
    flag_(lock.flag_)
{
    while(flag_.test_and_set(std::memory_order::memory_order_acquire)) {}
}

AtomicMutex::LockGuard::~LockGuard()
{
    flag_.clear(std::memory_order::memory_order_release);
}
}