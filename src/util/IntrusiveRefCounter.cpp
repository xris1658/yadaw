#include "IntrusiveRefCounter.hpp"

namespace YADAW::Util
{
IntrusiveRefCounter::IntrusiveRefCounter():
    refCount_(1U)
{}

IntrusiveRefCounter::~IntrusiveRefCounter()
{}

void IntrusiveRefCounter::addRef()
{
    if(auto refCount = refCount_.load(std::memory_order::memory_order_acquire);
        refCount != 0U)
    {
        refCount_.store(refCount + 1, std::memory_order::memory_order_relaxed);
    }
}

void IntrusiveRefCounter::release()
{
    if(auto refCount = refCount_.fetch_sub(1, std::memory_order::memory_order_seq_cst);
        refCount == 1U)
    {
        delete this;
    }
}
}