#ifndef YADAW_SRC_NATIVE_THREAD
#define YADAW_SRC_NATIVE_THREAD

#if _WIN32
#include <windows.h>
#elif __linux__

#endif

#include <cstdint>
#include <thread>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity);

std::thread::native_handle_type getCurrentThreadHandle();

inline void inSpinLockLoop()
{
#if _WIN32
    YieldProcessor();
#elif __linux__
    // x86: __builtin_ia32_pause();
    // ARM: __yield();
#endif
}
}

#endif // YADAW_SRC_NATIVE_THREAD