#if _WIN32

#include "concurrent/InterProcessMutex.hpp"
#include "native/win/InterProcessMutexImpl.hpp"

namespace YADAW::Concurrent
{
InterProcessMutex::InterProcessMutex(void* mutexID):
    pImpl_(std::make_unique<Impl>(mutexID))
{}

InterProcessMutex::~InterProcessMutex()
{}

void InterProcessMutex::lock()
{
    pImpl_->lock();
}

bool InterProcessMutex::try_lock()
{
    return pImpl_->try_lock();
}

void InterProcessMutex::unlock()
{
    pImpl_->unlock();
}
}

#endif