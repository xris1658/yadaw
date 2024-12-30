#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <cinttypes>
#include <cstdint>
#include <cstdio>

int main()
{
    auto numaNodes = YADAW::Native::getCPUTopology();
    for(const auto& [numaNodeIndex, packages]: numaNodes)
    {
        std::printf("NUMA Node #%d\n", static_cast<int>(numaNodeIndex) + 1);
        for(const auto& [package, physicalCores]: packages)
        {
            std::printf("  Package #%d\n", static_cast<int>(package) + 1);
            for(const auto& [physicalCore, logicalProcessors]: physicalCores)
            {
                std::printf("    Physical Core #%d\n", static_cast<int>(physicalCore) + 1);
                for(auto logicalProcessor: logicalProcessors)
                {
                    std::printf("      Logical Processor #%d\n", static_cast<int>(logicalProcessor) + 1);
                }
            }
        }
    }

}