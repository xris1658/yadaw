#include "AudioDeviceGraphWithPDC.hpp"

namespace YADAW::Audio::Engine
{
AudioDeviceGraphWithPDC::AudioDeviceGraphWithPDC(AudioDeviceGraphBase& graph,
    Extension::UpstreamLatency& upstreamLatency):
    graph_(graph),
    upstreamLatency_(upstreamLatency),
    multiInputNodes_()
{
    using YADAW::Audio::Engine::Extension::UpstreamLatency;
    using YADAW::Audio::Engine::MultiInputDeviceWithPDC;
    upstreamLatency_.setLatencyOfNodeUpdatedCallback(
        [this](const UpstreamLatency& sender, const ade::NodeHandle& nodeHandle)
        {
            onLatencyOfNodeUpdated(nodeHandle);
        }
    );
}

AudioDeviceGraphWithPDC::~AudioDeviceGraphWithPDC()
{
    using YADAW::Audio::Engine::Extension::UpstreamLatency;
    upstreamLatency_.resetLatencyOfNodeUpdatedCallback();
    for(auto& [nodeHandle, pdc]: multiInputNodes_)
    {
        graph_.removeNode(nodeHandle);
    }
    multiInputNodes_.clear();
}

ade::NodeHandle AudioDeviceGraphWithPDC::addNode(AudioDeviceProcess&& process)
{
    using YADAW::Audio::Engine::AudioDeviceProcess;
    using YADAW::Audio::Engine::MultiInputDeviceWithPDC;
    auto device = process.device();
    if(device->audioInputGroupCount() > 1)
    {
        auto deviceWithPDC = std::make_unique<MultiInputDeviceWithPDC>(
            std::move(process)
        );
        auto ret = graph_.addNode(AudioDeviceProcess(*deviceWithPDC));
        multiInputNodes_.emplace(ret, std::move(deviceWithPDC));
        return ret;
    }
    return graph_.addNode(std::move(process));
}

std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC> AudioDeviceGraphWithPDC::removeNode(const ade::NodeHandle& nodeHandle)
{
    if(auto device = graph_.getNodeData(nodeHandle).process.device();
        device->audioInputGroupCount() > 1)
    {
        auto it = multiInputNodes_.find(nodeHandle);
        if(it != multiInputNodes_.end())
        {
            auto ret = std::move(it->second);
            auto node = nodeHandle;
            multiInputNodes_.erase(it);
            graph_.removeNode(node);
            return ret;
        }
    }
    graph_.removeNode(nodeHandle);
    return nullptr;
}

void AudioDeviceGraphWithPDC::clearMultiInputNodes()
{
    auto count = multiInputNodes_.size();
    FOR_RANGE0(i, count)
    {
        auto multiInputNode = removeNode(multiInputNodes_.begin()->first);
    }
}

YADAW::Audio::Engine::AudioDeviceGraphBase& AudioDeviceGraphWithPDC::graph() const
{
    return graph_;
}

YADAW::Audio::Engine::Extension::UpstreamLatency& AudioDeviceGraphWithPDC::upstreamLatencyExtension() const
{
    return upstreamLatency_;
}

void AudioDeviceGraphWithPDC::onLatencyOfNodeUpdated(const ade::NodeHandle& nodeHandle)
{
    auto device = graph_.getNodeData(nodeHandle).process.device();
    if(auto inputCount = device->audioInputGroupCount(); inputCount > 1)
    {
        auto it = multiInputNodes_.find(nodeHandle);
        if(it != multiInputNodes_.end())
        {
            auto& [node, pdc] = *it;
            auto maximumLatency = upstreamLatency_.getMaxUpstreamLatency(nodeHandle);
            FOR_RANGE0(i, inputCount)
            {
                pdc->setDelayOfPDC(i, maximumLatency - upstreamLatency_.getUpstreamLatency(nodeHandle, i).value());
            }
        }
    }
}
}