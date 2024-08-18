#if _WIN32

#include "InterProcessMutexImpl.hpp"

namespace YADAW::Concurrent
{
InterProcessMutex::Impl::Impl(void* mutexId):
    mutexId_(static_cast<LPCWSTR>(mutexId))
{}

InterProcessMutex::Impl::~Impl()
{
    unlock();
}

void InterProcessMutex::Impl::lock()
{
    mutex_ = CreateMutexW(
        NULL, FALSE,
        L"GLOBAL_YADAW_INSTANCE"
    );
    auto errorCode = GetLastError();
    if(errorCode == ERROR_ALREADY_EXISTS)
    {
        WaitForSingleObjectEx(mutex_, INFINITE, FALSE);
    }
    else if(errorCode != ERROR_SUCCESS)
    {
        throw std::runtime_error("Failed to lock mutex");
    }
}

bool InterProcessMutex::Impl::try_lock()
{
    mutex_ = CreateMutexW(NULL, FALSE, mutexId_);
    auto errorCode = GetLastError();
    if(errorCode == ERROR_SUCCESS)
    {
        return true;
    }
    else if(errorCode == ERROR_ALREADY_EXISTS)
    {
        mutex_ = NULL;
        return false;
    }
    throw std::runtime_error("Failed to lock mutex");
}

void InterProcessMutex::Impl::unlock()
{
    if(mutex_)
    {
        ReleaseMutex(mutex_);
        mutex_ = NULL;
    }
}
}

#endif