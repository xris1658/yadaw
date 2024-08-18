#ifndef YADAW_SRC_NATIVE_WIN_INTERPROCESSMUTEXIMPL
#define YADAW_SRC_NATIVE_WIN_INTERPROCESSMUTEXIMPL

#if _WIN32

#include "concurrent/InterProcessMutex.hpp"
#include "native/Native.hpp"

#include <Windows.h>

class YADAW::Concurrent::InterProcessMutex::Impl
{
public:
    Impl(void* mutexId);
    ~Impl();
public:
    void lock();
    bool try_lock();
    void unlock();
private:
    LPCWSTR mutexId_;
    HANDLE mutex_ = NULL;
};

#endif

#endif // YADAW_SRC_NATIVE_WIN_INTERPROCESSMUTEXIMPL