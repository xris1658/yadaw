#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <cinttypes>
#include <cstdint>
#include <cstdio>

int main()
{
    auto numaNodes = YADAW::Native::getCPUHierarchy();
    std::uint64_t coreCount = 0;
    std::uint64_t threadCount = 0;
    FOR_RANGE0(i, numaNodes.size())
    {
        std::printf("[-] NUMA Node #%" PRIu64 "\n", i + 1);
        const auto& processors = numaNodes[i];
        FOR_RANGE0(j, processors.size())
        {
            std::printf(" +--[-] Processor Package #%" PRIu64 "\n", j + 1);
            const auto& physicalCores = processors[j];
            FOR_RANGE0(k, physicalCores.size())
            {
                ++coreCount;
                threadCount += physicalCores[k];
                std::printf(" |   +---Physical Core #%" PRIu64 ": %" PRIu8 " cores\n", k + 1, physicalCores[k]);
            }
        }
    }
    std::printf("%" PRIu64 " cores, %" PRIu64 " threads in total\n", coreCount, threadCount);
}