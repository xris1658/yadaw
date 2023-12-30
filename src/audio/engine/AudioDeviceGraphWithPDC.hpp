#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHWITHPDC
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHWITHPDC

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "util/ADEUtil.hpp"

#include <unordered_map>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraphWithPDC
{
public:
    AudioDeviceGraphWithPDC(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        YADAW::Audio::Engine::Extension::UpstreamLatency& upstreamLatency);
    template<typename... Extensions>
    AudioDeviceGraphWithPDC(YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph):
        AudioDeviceGraphWithPDC(
            graph,
            graph.template getExtension<YADAW::Audio::Engine::Extension::UpstreamLatency>()
        )
    {}
    AudioDeviceGraphWithPDC(const AudioDeviceGraphWithPDC&) = delete;
    AudioDeviceGraphWithPDC(AudioDeviceGraphWithPDC&&) = delete;
    ~AudioDeviceGraphWithPDC();
public:
    ade::NodeHandle addNode(YADAW::Audio::Engine::AudioDeviceProcess&& process);
    void removeNode(const ade::NodeHandle& nodeHandle);
public:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph() const;
    YADAW::Audio::Engine::Extension::UpstreamLatency& upstreamLatency() const;
private:
    void onLatencyOfNodeUpdated(const ade::NodeHandle& nodeHandle);
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph_;
    YADAW::Audio::Engine::Extension::UpstreamLatency& upstreamLatency_;
    std::unordered_map<
        ade::NodeHandle,
        std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>,
        ade::HandleHasher<ade::Node>
    > multiInputNodes_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHWITHPDC
