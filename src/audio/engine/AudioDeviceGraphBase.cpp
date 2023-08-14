#include "AudioDeviceGraphBase.hpp"

#include "util/ADEUtil.hpp"

#include <deque>

using YADAW::Audio::Util::SampleDelay;

namespace YADAW::Audio::Engine
{

AudioDeviceGraphBase::AudioDeviceGraphBase():
    graph_(),
    typedGraph_(graph_) {}

const AudioDeviceGraphBase::AudioDeviceProcessNode&
AudioDeviceGraphBase::getMetadataFromNode(const ade::NodeHandle& nodeHandle) const
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

AudioDeviceGraphBase::AudioDeviceProcessNode&
AudioDeviceGraphBase::getMetadataFromNode(ade::NodeHandle& nodeHandle)
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

void AudioDeviceGraphBase::setMetadataFromNode(
    ade::NodeHandle& nodeHandle, AudioDeviceGraphBase::AudioDeviceProcessNode&& metadata)
{
    typedGraph_.metadata(nodeHandle).set<AudioDeviceProcessNode>(std::move(metadata));
}

ade::NodeHandle AudioDeviceGraphBase::addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto nodeHandle = graph_.createNode();
    setMetadataFromNode(nodeHandle, AudioDeviceProcessNode{std::move(process), std::move(audioProcessData)});
    return nodeHandle;
}

void AudioDeviceGraphBase::removeNode(ade::NodeHandle nodeHandle)
{
    typedGraph_.metadata(nodeHandle).erase<AudioDeviceProcessNode>();
    typedGraph_.erase(nodeHandle);
}

ade::EdgeHandle AudioDeviceGraphBase::connect(ade::NodeHandle from, ade::NodeHandle to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto edgeHandle = typedGraph_.link(from, to);
    return edgeHandle;
}

void AudioDeviceGraphBase::disconnect(ade::EdgeHandle edgeHandle)
{
    typedGraph_.metadata(edgeHandle->dstNode()).get<AudioDeviceProcessNode>().upstreamLatency = 0U;
    typedGraph_.erase(edgeHandle);
}
}