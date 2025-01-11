#ifndef YADAW_SRC_NATIVE_THREAD
#define YADAW_SRC_NATIVE_THREAD

#if _WIN32
#include <windows.h>
#elif __linux__
#include <xmmintrin.h>
#endif

#include <cstdint>
#include <thread>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity);

std::thread::native_handle_type getCurrentThreadHandle();

void setThreadPriorityToTimecritical(std::thread::native_handle_type handle);

inline void inSpinLockLoop()
{
#if _WIN32
    YieldProcessor();
#elif __linux__
#if __x86_64__
    _mm_pause();
#endif
#endif
}
}

#endif // YADAW_SRC_NATIVE_THREAD