#ifndef YADAW_SRC_CONCURRENT_INTERPROCESSMUTEX
#define YADAW_SRC_CONCURRENT_INTERPROCESSMUTEX

#include <memory>

namespace YADAW::Concurrent
{
class InterProcessMutex
{
private:
    class Impl;
public:
    using MutexID = void*;
public:
    InterProcessMutex(void* mutexID);
    ~InterProcessMutex();
public:
    void lock();
    bool try_lock();
    void unlock();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif // YADAW_SRC_CONCURRENT_INTERPROCESSMUTEX
