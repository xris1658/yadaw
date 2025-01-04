#if _WIN32

#include "native/Thread.hpp"

#include <windows.h>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity)
{
    return SetThreadAffinityMask(handle, affinity);
}

std::thread::native_handle_type getCurrentThreadHandle()
{
    return GetCurrentThread();
}
}

#endif