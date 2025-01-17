#if __APPLE__

#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <sys/types.h>
#include <sys/sysctl.h>

#include <cstdint>

namespace YADAW::Native
{
CPUTopology getCPUTopology()
{
    constexpr char hwPhysicalCoreCount[] = "hw.physicalcpu"; // int32_t
    constexpr char hwLogicalCoreCount[] = "hw.logicalcpu";   // int32_t
    std::int32_t packageCount = 0;
    std::size_t size = sizeof(std::int32_t);
    sysctlbyname(
        "hw.packages",
        &packageCount, &size, nullptr, 0
    );
    std::int32_t physicalCoreCount = 0;
    sysctlbyname(
        "hw.physicalcpu",
        &physicalCoreCount, &size, nullptr, 0
    );
    std::int32_t logicalProcessorCount = 0;
    sysctlbyname(
        "hw.logicalcpu",
        &logicalProcessorCount, &size, nullptr, 0
    );
    CPUTopology ret;
    FOR_RANGE0(i, packageCount)
    {
        ret[i];
    }
    return ret;
}

std::uint64_t getProcessAffinity()
{
    // TODO
    return 0;
}
}

#endif