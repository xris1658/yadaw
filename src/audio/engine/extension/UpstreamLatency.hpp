#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_UPSTREAMLATENCY
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_UPSTREAMLATENCY

#include "audio/engine/AudioDeviceGraph.hpp"

namespace YADAW::Audio::Engine::Extension
{
class UpstreamLatency
{
public:
    struct DataType
    {
        std::uint32_t upstreamLatency;
    };
public:
    UpstreamLatency(AudioDeviceGraphBase& graph,
        DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&));
private:
    std::uint32_t sumLatency(const ade::NodeHandle& noeHandle);
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
public:
    std::uint32_t getUpstreamLatency(const ade::NodeHandle& nodeHandle);
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_EXTENSION_UPSTREAMLATENCY
