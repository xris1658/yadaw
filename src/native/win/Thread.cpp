#if _WIN32

#include "native/Thread.hpp"

#include <windows.h>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity)
{
    return SetThreadAffinityMask(reinterpret_cast<HANDLE>(handle), affinity);
}

std::thread::native_handle_type getCurrentThreadHandle()
{
    return reinterpret_cast<std::thread::native_handle_type>(GetCurrentThread());
}

void setThreadPriorityToTimecritical(std::thread::native_handle_type handle)
{
    SetThreadPriority(reinterpret_cast<HANDLE>(handle), THREAD_PRIORITY_TIME_CRITICAL);
}
}

#endif