#if _WIN32

#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <intrin.h>

#include <windows.h>
#include <sysinfoapi.h>

#include <bitset>
#include <memory>

inline WORD getNUMANodeGroupCount(const NUMA_NODE_RELATIONSHIP& numaNodeRelationship)
{
    // NUMA node group count is introduced in Windows 10 20H2 (build 19042).
    // This field does not exist in Windows SDK 10.0.19041.0 (but it exists in
    // newer SDKs):
    //
    //   File: winnt.h (older SDK)
    //   BYTE  Reserved[20];
    //
    //   winnt.h (newer SDK)
    //   BYTE Reserved[18];
    //   WORD GroupCount;
    //
    // The following is my way of grabbing group count. I believe this way works
    // with both newer and older Windows SDKs.
    return *reinterpret_cast<const WORD*>(numaNodeRelationship.Reserved + 18);
}

std::pair<std::unique_ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>, DWORD> getLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP relationship)
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer = nullptr;
    DWORD length = 0;
    while(GetLogicalProcessorInformationEx(relationship, buffer, &length) == FALSE)
    {
        if(auto errorCode = GetLastError(); errorCode == ERROR_INSUFFICIENT_BUFFER)
        {
            if(buffer)
            {
                std::free(buffer);
            }
            buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
                std::malloc(length)
            );
            if(!buffer)
            {
                return {};
            }
        }
        else
        {
            return {};
        }
    }
    return std::make_pair(std::unique_ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer), length);
}

namespace YADAW::Native
{
CPUTopology getCPUTopology()
{
    OSVERSIONINFOW versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionExW(&versionInfo);
    bool isSince20H2 = versionInfo.dwBuildNumber >= 19042;
    std::map<std::uint8_t, std::set<std::uint16_t>> numaNodeProcessors;
    std::map<std::uint8_t, std::set<std::uint16_t>> packageProcessors;
    std::map<std::uint8_t, std::set<std::uint16_t>> physicalCoreProcessors;
    CPUTopology ret;
    std::pair<std::unique_ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>, DWORD> pair;
    // Get NUMA nodes
    pair = getLogicalProcessorInformationEx(RelationNumaNode);
    if(auto& [numaNodes, bufferSize] = pair; numaNodes)
    {
        auto buffer = numaNodes.get();
        for(std::byte* ptr = reinterpret_cast<std::byte*>(buffer);
            ptr < reinterpret_cast<std::byte*>(buffer) + bufferSize;)
        {
            auto infoEx = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
            const auto& numaNode = infoEx->NumaNode;
            auto nodeIndex = numaNode.NodeNumber;
            auto fallback = true;
            auto groupCount = 1;
            if(isSince20H2)
            {
                if(auto actualGroupCount = getNUMANodeGroupCount(numaNode); actualGroupCount)
                {
                    groupCount = actualGroupCount;
                }
            }
            FOR_RANGE0(i, groupCount)
            {
                const auto& groupAffinity = (&numaNode.GroupMask)[i];
                auto groupIndex = groupAffinity.Group;
                const auto& mask = groupAffinity.Mask;
                std::bitset<64> bitset(mask);
                FOR_RANGE0(j, bitset.size())
                {
                    if(bitset.test(j))
                    {
                        numaNodeProcessors[nodeIndex].emplace(
                            groupIndex * 64 + j
                        );
                    }
                }
            }
            ptr += infoEx->Size;
        }
    }
    pair = getLogicalProcessorInformationEx(RelationProcessorPackage);
    if(auto& [packages, bufferSize] = pair; packages)
    {
        auto buffer = packages.get();
        auto packageIndex = 0;
        for(std::byte* ptr = reinterpret_cast<std::byte*>(buffer);
            ptr < reinterpret_cast<std::byte*>(buffer) + bufferSize;)
        {
            auto infoEx = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
            const auto& package = infoEx->Processor;
            FOR_RANGE0(i, package.GroupCount)
            {
                const auto& groupAffinity = package.GroupMask[i];
                auto groupIndex = groupAffinity.Group;
                const auto& mask = groupAffinity.Mask;
                std::bitset<64> bitset(mask);
                FOR_RANGE0(j, bitset.size())
                {
                    if(bitset.test(j))
                    {
                        packageProcessors[packageIndex].emplace(
                            groupIndex * 64 + j
                        );
                    }
                }
            }
            ++packageIndex;
            ptr += infoEx->Size;
        }
    }
    pair = getLogicalProcessorInformationEx(RelationProcessorCore);
    if(auto& [cores, bufferSize] = pair; cores)
    {
        auto buffer = cores.get();
        auto coreIndex = 0;
        for(std::byte* ptr = reinterpret_cast<std::byte*>(buffer);
            ptr < reinterpret_cast<std::byte*>(buffer) + bufferSize;)
        {
            auto infoEx = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
            const auto& core = infoEx->Processor;
            FOR_RANGE0(i, core.GroupCount)
            {
                const auto& groupAffinity = core.GroupMask[i];
                auto groupIndex = groupAffinity.Group;
                const auto& mask = groupAffinity.Mask;
                std::bitset<64> bitset(mask);
                FOR_RANGE0(j, bitset.size())
                {
                    if(bitset.test(j))
                    {
                        physicalCoreProcessors[coreIndex].emplace(
                            groupIndex * 64 + j
                        );
                    }
                }
            }
            ++coreIndex;
            ptr += infoEx->Size;
        }
    }
    struct Belonging
    {
        std::uint8_t numaNodeIndex = 0xff;
        std::uint8_t packageIndex = 0xff;
        std::uint8_t physicalCoreIndex = 0xff;
    };
    std::map<std::uint16_t, Belonging> logicalProcessorBelongings;
    for(const auto& [numaNode, processors]: numaNodeProcessors)
    {
        for(auto processor: processors)
        {
            logicalProcessorBelongings[processor].numaNodeIndex = numaNode;
        }
    }
    for(const auto& [package, processors]: packageProcessors)
    {
        for(auto processor: processors)
        {
            logicalProcessorBelongings[processor].packageIndex = package;
        }
    }
    for(const auto& [physicalCore, processors]: physicalCoreProcessors)
    {
        for(auto processor: processors)
        {
            logicalProcessorBelongings[processor].physicalCoreIndex = physicalCore;
        }
    }
    for(const auto& [logicalProcessor, belonging]: logicalProcessorBelongings)
    {
        const auto& [numaNodeIndex, packageIndex, physicalCoreIndex] = belonging;
        ret[numaNodeIndex][packageIndex][physicalCoreIndex].emplace(logicalProcessor);
    }
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