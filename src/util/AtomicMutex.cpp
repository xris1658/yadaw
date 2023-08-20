#include "AtomicMutex.hpp"

namespace YADAW::Util
{
void AtomicMutex::lock()
{
    while(flag_.test_and_set(std::memory_order::memory_order_acquire)) {}
}

bool AtomicMutex::try_lock()
{
    return !flag_.test_and_set(std::memory_order::memory_order_acquire);
}

void AtomicMutex::unlock()
{
    flag_.clear(std::memory_order::memory_order_release);
}
}