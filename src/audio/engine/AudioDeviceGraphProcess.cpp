#include "AudioDeviceGraphProcess.hpp"

namespace YADAW::Audio::Engine
{
using ProcessSequence = Vector3D<
    ProcessPair
>;
ProcessSequence getProcessSequence(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
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
                    std::make_pair(
                        std::move(graph.getNodeData(tijk).process),
                        bufferExt.getData(tijk).container.audioProcessData()
                    )
                );
            }
        }
    }
    return ret;
}
}