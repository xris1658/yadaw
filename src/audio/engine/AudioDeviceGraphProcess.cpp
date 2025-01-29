#include "AudioDeviceGraphProcess.hpp"

#include <numeric>

namespace YADAW::Audio::Engine
{
using ProcessSequence = Vector3D<
    ProcessPair
>;
ProcessSequence getProcessSequence(const YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    YADAW::Audio::Engine::Extension::Buffer& bufferExt)
{
    auto topoSequence = graph.topologicalSort();
    ProcessSequence ret;
    const auto size = topoSequence.size();
    ret.resize(size);
    FOR_RANGE0(i, size)
    {
        auto& ti = topoSequence[i];
        const auto size = ti.size();
        auto& ri = ret[i];
        ri.resize(size);
        FOR_RANGE0(j, size)
        {
            auto& tij = ti[j];
            const auto size = tij.size();
            auto& rij = ri[j];
            ri[j].reserve(size);
            FOR_RANGE0(k, size)
            {
                auto& tijk = tij[k];
                rij.emplace_back(
                    graph.getNodeData(tijk).process,
                    bufferExt.getData(tijk).container
                );
            }
        }
    }
    return ret;
}

ProcessSequenceWithPrev getProcessSequenceWithPrev(
    const YADAW::Audio::Engine::AudioDeviceGraphBase& graph, YADAW::Audio::Engine::Extension::Buffer& bufferExt)
{
    auto topoSequenceWithPrev = graph.topologicalSortWithPrev();
    ProcessSequenceWithPrev ret;
    const auto size = topoSequenceWithPrev.size();
    ret.resize(size);
    FOR_RANGE0(i, size)
    {
        auto& ti = topoSequenceWithPrev[i];
        const auto size = ti.size();
        auto& ri = ret[i];
        ri.resize(size);
        FOR_RANGE0(j, size)
        {
            auto& [link, prev] = ti[j];
            auto& [retLink, retPrev] = ri[j];
            retLink.reserve(link.size());;
            retPrev.reserve(prev.size());
            FOR_RANGE0(k, link.size())
            {
                auto& tijk = link[k];
                retLink.emplace_back(
                    graph.getNodeData(tijk).process,
                    bufferExt.getData(tijk).container
                );
            }
            FOR_RANGE0(k, prev.size())
            {
                retPrev.emplace_back(prev[k]);
            }
        }
    }
    return ret;
}

Vec<AudioThreadWorkload> createWorkload(
    const ProcessSequenceWithPrev& processSequenceWithPrev,
    std::uint32_t threadCount,
    CreateWorkloadFlags)
{
    if(threadCount == 0)
    {
        threadCount = 1;
    }
    if(processSequenceWithPrev.empty())
    {
        return Vec<AudioThreadWorkload>(threadCount);
    }
    using Row = ProcessSequenceWithPrev::value_type;
    Vec<AudioThreadWorkload> ret;
    ret.resize(threadCount);
    decltype(threadCount) remOffset = 0;
    FOR_RANGE0(i, processSequenceWithPrev.size())
    {
        const auto& row = processSequenceWithPrev[i];
        auto [quot, rem] = std::ldiv(row.size(), threadCount);
        FOR_RANGE0(j, threadCount)
        {
            FOR_RANGE0(k, quot)
            {
                ret[j].emplace_back(
                    i, k * threadCount + j
                );
            }
        }
        FOR_RANGE(j, quot * threadCount, row.size())
        {
            ret[remOffset++].emplace_back(
                i, j
            );
            remOffset -= threadCount * (remOffset == threadCount);
        }
    }
    return ret;
}
}
