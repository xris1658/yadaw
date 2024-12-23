#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS

#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "util/IntegerRange.hpp"
#include "util/VectorTypes.hpp"

#include <vector>

namespace YADAW::Audio::Engine
{
using ProcessPair = std::pair<
    YADAW::Audio::Engine::AudioDeviceProcess,
    YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
>;
using ProcessSequence = Vector3D<
    ProcessPair
>;

using ProcessSequenceWithPrev = Vec<Vec<Vec<ProcessPair>, Vec<std::uint32_t, std::uint32_t>>>;

using AudioThreadWorkload = Vec<std::uint32_t, std::uint32_t>;

template<typename... Extensions>
ProcessSequence getProcessSequence(const YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph)
{
    return getProcessSequence(graph,
        graph.template getExtension<YADAW::Audio::Engine::Extension::Buffer>()
    );
}

template<typename... Extensions>
ProcessSequenceWithPrev getProcessSequenceWithPrev(const YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph)
{
    return getProcessSequenceWithPrev(graph,
        graph.template getExtension<YADAW::Audio::Engine::Extension::Buffer>()
    );
}

ProcessSequence getProcessSequence(const YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    YADAW::Audio::Engine::Extension::Buffer& bufferExt);

ProcessSequenceWithPrev getProcessSequenceWithPrev(
    const YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    YADAW::Audio::Engine::Extension::Buffer& bufferExt);

enum CreateWorkloadFlags
{
    UseAllThreads = 1 << 0
};

Vec<AudioThreadWorkload> createWorkload(
    const ProcessSequenceWithPrev& processSequenceWithPrev,
    std::uint32_t threadCount,
    CreateWorkloadFlags flags);
}



#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHPROCESS
