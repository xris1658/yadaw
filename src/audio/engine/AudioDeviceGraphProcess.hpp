#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS

#include "audio/engine/extension/Buffer.hpp"
#include "util/IntegerRange.hpp"

#include <vector>

namespace YADAW::Audio::Engine
{
template<typename T>
using Vector3D = std::vector<std::vector<std::vector<T>>>;
template<typename... Extensions>
Vector3D<
    std::pair<
        YADAW::Audio::Engine::AudioDeviceProcess,
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
    >
> getProcessSequence(YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph)
{
    auto topoSequence = graph.topologicalSort();
    Vector3D<
        std::pair<
            YADAW::Audio::Engine::AudioDeviceProcess,
            YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
        >
    > ret;
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
                        graph.getNodeData(tijk),
                        graph.template getExtensionData<YADAW::Audio::Engine::Extension::Buffer>(tijk)
                    )
                );
            }
        }
    }
    return ret;
}
}

#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS
