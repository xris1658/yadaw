#ifndef YADAW_SRC_NATIVE_CPU
#define YADAW_SRC_NATIVE_CPU

#include "util/VectorTypes.hpp"

#include <cstdint>
#include <map>
#include <set>

#if _WIN32
#include <windows.h>
#endif

namespace YADAW::Native
{
using CPUTopology = std::map<
    std::uint8_t, std::map< // NUMA nodes
        std::uint8_t, std::map< // CPU package
            std::uint8_t, std::set< // physical core
                std::uint16_t // logical processor
            >
        >
    >
>;

CPUTopology getCPUTopology();

std::uint64_t getProcessAffinity();

extern std::uint32_t cpuidInfo[4];

void fillCPUIDInfo();
}

#endif // YADAW_SRC_NATIVE_CPU