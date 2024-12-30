#if __linux__

#include "native/CPU.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"
#include "util/VectorTypes.hpp"

#include <cpuid.h>

#include <charconv>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string_view>

namespace YADAW::Native
{
// Parse range expression like "0-10,15".
Vec<YADAW::Util::IntegerRange<int>> parseRangesFromStream(std::istream& ifs)
{
    Vec<YADAW::Util::IntegerRange<int>> ret;
    while(true)
    {
        int index = -1;
        ifs >> index;
        if(auto peek = ifs.peek(); peek == '-')
        {
            ifs.get(); // '-'
            int index2 = -1;
            ifs >> index2;
            ret.emplace_back(index, index2 + 1);
        }
        else
        {
            ret.emplace_back(index, index + 1);
        }
        if(ifs.get() == '\n') // ',' or '\n'
        {
            break;
        }
    }
    return ret;
}

const char nodeText[] = "node";
const char cpuText [] = "cpu";

CPUTopology getCPUTopology()
{
    namespace fs = std::filesystem;
    std::error_code ec;
    std::map<std::uint8_t, Vec<std::uint8_t>> logicalProcessorsInNUMANode;
    std::map<std::uint8_t, std::set<std::uint8_t>> logicalProcessorsInPhysicalCore;
    std::map<std::uint8_t, std::set<std::uint8_t>> logicalProcessorsInPackage;
    std::string path;
    char numberBuffer[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // std::size("4294967295") + 1
    if(std::ifstream ifs("/sys/devices/system/node/online"); ifs.is_open())
    {
        auto numaNodeRanges = parseRangesFromStream(ifs);
        path.reserve(
            YADAW::Util::stringLength("/sys/devices/system/node/node")
            + std::size(numberBuffer)
            + YADAW::Util::stringLength("/cpulist")
        );
        path.append("/sys/devices/system/node/node");
        for(const auto& numaNodeRange: numaNodeRanges)
        {
            for(auto numaNodeIndex: numaNodeRange)
            {
                if(auto [ptr, ec2] = std::to_chars(numberBuffer, numberBuffer + std::size(numberBuffer), numaNodeIndex); !static_cast<bool>(ec2))
                {
                    auto indexView = std::string_view(numberBuffer, ptr - numberBuffer);
                    path.append(indexView);
                    path.append("/cpulist");
                    if(std::ifstream ifs(path); ifs.is_open())
                    {
                        auto& nodeMapIt = logicalProcessorsInNUMANode[numaNodeIndex];
                        auto cpuRanges = parseRangesFromStream(ifs);
                        for(const auto& cpuRange: cpuRanges)
                        {
                            for(auto cpuIndex: cpuRange)
                            {
                                nodeMapIt.emplace_back(cpuIndex);
                            }
                        }
                    }
                    path.resize(YADAW::Util::stringLength("/sys/devices/system/node/node"));
                }
            }
        }
    }
    // Get a list of online logical processors, and physical cores, CPU packages they belong
    if(std::ifstream ifs("/sys/devices/system/cpu/online"); ifs.is_open())
    {
        path.reserve(
            YADAW::Util::stringLength("/sys/devices/system/cpu/cpu")
            + std::size(numberBuffer)
            + YADAW::Util::stringLength("/topology/physical_package_id")
        );
        auto cpuRanges = parseRangesFromStream(ifs);
        path.resize(YADAW::Util::stringLength("/sys/devices/system/"));
        path.append("cpu/cpu");
        auto prefixLength = path.size();
        for(const auto& cpuRange: cpuRanges)
        {
            for(auto cpuIndex: cpuRange)
            {
                path.resize(prefixLength);
                if(auto [ptr, ec2] = std::to_chars(numberBuffer, numberBuffer + std::size(numberBuffer), cpuIndex); !static_cast<bool>(ec2))
                {
                    auto view = std::string_view(numberBuffer, ptr - numberBuffer);
                    path.append(view);
                    path.append("/topology/");
                    auto size = path.size();
                    path.append("core_id");
                    if(auto ifs = std::ifstream(path); ifs.is_open())
                    {
                        int physicalCoreId = -1;
                        ifs >> physicalCoreId;
                        ifs.close();
                        path.resize(size);
                        path.append("core_cpus_list");
                        auto& logicalProcessors = logicalProcessorsInPhysicalCore[physicalCoreId];
                        if(auto ifs = std::ifstream(path); ifs.is_open())
                        {
                            auto logicalProcessorRanges = parseRangesFromStream(ifs);
                            ifs.close();
                            for(const auto& logicalProcessorRange: logicalProcessorRanges)
                            {
                                for(auto logicalProcessorIndex: logicalProcessorRange)
                                {
                                    logicalProcessors.emplace(logicalProcessorIndex);
                                }
                            }
                        }
                    }
                    path.resize(size);
                    path.append("physical_package_id");
                    if(auto ifs = std::ifstream(path); ifs.is_open())
                    {
                        int packageId = -1;
                        ifs >> packageId;
                        ifs.close();
                        path.resize(size);
                        path.append("package_cpus_list");
                        auto& logicalProcessors = logicalProcessorsInPackage[packageId];
                        if(auto ifs = std::ifstream(path); ifs.is_open())
                        {
                            auto logicalProcessorRanges = parseRangesFromStream(ifs);
                            ifs.close();
                            for(const auto& logicalProcessorRange: logicalProcessorRanges)
                            {
                                for(auto logicalProcessorIndex: logicalProcessorRange)
                                {
                                    logicalProcessors.emplace(logicalProcessorIndex);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    CPUTopology ret;
    struct Belonging
    {
        std::uint8_t numaNodeIndex = 0xff;
        std::uint8_t packageIndex = 0xff;
        std::uint8_t physicalCoreIndex = 0xff;
    };
    std::map<std::uint16_t, Belonging> logicalProcessors;
    for(const auto& [numaNode, processors]: logicalProcessorsInNUMANode)
    {
        for(auto processor: processors)
        {
            logicalProcessors[processor].numaNodeIndex = numaNode;
        }
    }
    for(const auto& [package, processors]: logicalProcessorsInPackage)
    {
        for(auto processor: processors)
        {
            logicalProcessors[processor].packageIndex = package;
        }
    }
    for(const auto& [physicalCore, processors]: logicalProcessorsInPhysicalCore)
    {
        for(auto processor: processors)
        {
            logicalProcessors[processor].physicalCoreIndex = physicalCore;
        }
    }
    for(const auto& [logicalProcessor, belonging]: logicalProcessors)
    {
        const auto& [numaNode, package, physicalCore] = belonging;
        ret[numaNode][package][physicalCore].emplace(logicalProcessor);
    }
    return ret;
}

std::uint32_t cpuidInfo[4];

void fillCPUIDInfo()
{
    __get_cpuid(1, cpuidInfo, cpuidInfo + 1, cpuidInfo + 2, cpuidInfo + 3);
}
}

#endif