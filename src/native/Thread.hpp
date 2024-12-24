#ifndef YADAW_SRC_NATIVE_THREAD
#define YADAW_SRC_NATIVE_THREAD

#include <cstdint>
#include <thread>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity);
}

#endif // YADAW_SRC_NATIVE_THREAD