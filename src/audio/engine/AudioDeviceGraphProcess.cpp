#include "AudioDeviceGraphProcess.hpp"

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
}
