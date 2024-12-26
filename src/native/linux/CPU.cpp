#if __linux__

#include "native/CPU.hpp"

#include <cpuid.h>

namespace YADAW::Native
{
std::uint32_t cpuidInfo[4];

void fillCPUIDInfo()
{
    __get_cpuid(1, cpuidInfo, cpuidInfo + 1, cpuidInfo + 2, cpuidInfo + 3);
}
}

#endif