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
            std::printf(" %c--[-] Processor Package #%" PRIu64 "\n",
                j == processors.size() - 1? '\'': '|',
                j + 1);
            const auto& physicalCores = processors[j];
            FOR_RANGE0(k, physicalCores.size())
            {
                ++coreCount;
                auto& logicalProcessors = physicalCores[k];
                threadCount += logicalProcessors.size();
                std::printf(" %c   %c--[-] Physical Core #%" PRIu64 ":\n",
                    j == processors.size() - 1? ' ': '|',
                    k == physicalCores.size() - 1? '\'' : '|',
                    k + 1);
                FOR_RANGE0(l, physicalCores[k].size())
                {
                    auto logicalProcessor = logicalProcessors[l];
                    std::printf(" %c   %c   %c---Logical Processor %" PRIu8 "\n",
                        j == processors.size() - 1? ' ': '|',
                        k == physicalCores.size() - 1? ' ' : '|',
                        l == logicalProcessors.size() - 1? '\'': '|',
                        logicalProcessor + 1);
                }
            }
        }
    }
    std::printf("%" PRIu64 " cores, %" PRIu64 " threads in total\n", coreCount, threadCount);
}