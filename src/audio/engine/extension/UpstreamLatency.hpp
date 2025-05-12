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
        std::vector<std::uint32_t> upstreamLatencies;
    };
    using LatencyOfNodeUpdatedCallback = void(const UpstreamLatency& sender, const ade::NodeHandle& nodeHandle);
public:
    UpstreamLatency(AudioDeviceGraphBase& graph,
        DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&));
private:
    std::uint32_t sumLatency(const ade::NodeHandle& nodeHandle) const;
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
public:
    std::optional<std::uint32_t> getUpstreamLatency(
        const ade::NodeHandle& nodeHandle, std::uint32_t audioInputGroupIndex = 0U
    ) const;
    std::uint32_t getMaxUpstreamLatency(const ade::NodeHandle& nodeHandle) const;
    void setLatencyOfNodeUpdatedCallback(std::function<LatencyOfNodeUpdatedCallback> function);
    void resetLatencyOfNodeUpdatedCallback();
    void onLatencyOfNodeUpdated(const ade::NodeHandle& nodeHandle);
private:
    void updateUpstreamLatency(
        const ade::NodeHandle& outNode,
        std::optional<ade::EdgeHandle> edge,
        std::uint32_t audioInputGroupIndex);
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
    std::function<LatencyOfNodeUpdatedCallback> callback_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_UPSTREAMLATENCY
