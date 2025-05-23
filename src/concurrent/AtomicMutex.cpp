#include "AtomicMutex.hpp"

namespace YADAW::Concurrent
{
void AtomicMutex::lock()
{
    while(flag_.test_and_set(std::memory_order_acquire)) {}
}

bool AtomicMutex::try_lock()
{
    return !flag_.test_and_set(std::memory_order_acquire);
}

void AtomicMutex::unlock()
{
    flag_.clear(std::memory_order_release);
}
}