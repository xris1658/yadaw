#if _WIN32

#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <intrin.h>

#include <windows.h>
#include <sysinfoapi.h>

#include <bitset>

namespace YADAW::Native
{
// TODO: Need tests on environments with multiple processors, groups and NUMA nodes
CPUHierarchy getCPUHierarchy()
{
    CPUHierarchy ret;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
    DWORD length = 0;
    while(GetLogicalProcessorInformation(buffer, &length) == FALSE)
    {
        if(auto errorCode = GetLastError(); errorCode == ERROR_INSUFFICIENT_BUFFER)
        {
            if(buffer)
            {
                std::free(buffer);
            }
            buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(
                std::malloc(length)
            );
            if(!buffer)
            {
                return ret;
            }
        }
        else
        {
            return ret;
        }
    }
    auto count = length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    Vec<Vec<std::uint8_t>> physicalCores;
    Vec<decltype(physicalCores)> packages;
    for(auto ptr = buffer; ptr < buffer + count; ++ptr)
    {
        if(ptr->Relationship == RelationProcessorCore)
        {
            std::bitset<CHAR_BIT * sizeof(ptr->ProcessorMask)> bitset(ptr->ProcessorMask);
            auto& logicalProcessors = physicalCores.emplace_back();
            FOR_RANGE0(i, bitset.size())
            {
                if(bitset.test(i))
                {
                    logicalProcessors.emplace_back(i);
                }
            }
        }
        else if(ptr->Relationship == RelationProcessorPackage)
        {
            ++ptr;
            std::bitset<CHAR_BIT * sizeof(ptr->ProcessorMask)> bitset(ptr->ProcessorMask);
            auto& logicalProcessors = physicalCores.emplace_back();
            FOR_RANGE0(i, bitset.size())
            {
                if(bitset.test(i))
                {
                    logicalProcessors.emplace_back(i);
                }
            }
            packages.emplace_back(std::move(physicalCores));
        }
        else if(ptr->Relationship == RelationNumaNode)
        {
            ret.emplace_back(std::move(packages));
        }
        // FIXME: Handle other `LOGICAL_PROCESSOR_RELATIONSHIP` values
    }
    std::free(buffer);
    return ret;
}

std::uint64_t getProcessAffinity()
{
    DWORD_PTR processAffinity = 0;
    DWORD_PTR systemAffinity = 0;
    if(!GetProcessAffinityMask(
        GetCurrentProcess(), &processAffinity, &systemAffinity)
    )
    {
        processAffinity = 0;
    }
    return processAffinity;
}

std::uint32_t cpuidInfo[4];

void fillCPUIDInfo()
{
    __cpuid(reinterpret_cast<int(&)[4]>(cpuidInfo), 1);
}
}

#endif