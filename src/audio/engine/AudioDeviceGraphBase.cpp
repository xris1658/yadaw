#include "AudioDeviceGraphBase.hpp"

namespace YADAW::Audio::Engine
{

AudioDeviceGraphBase::AudioDeviceGraphBase():
    graph_(),
    typedGraph_(graph_) {}

ade::NodeHandle AudioDeviceGraphBase::addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto nodeHandle = typedGraph_.createNode();
    typedGraph_.metadata(nodeHandle).set<AudioDeviceProcessNode>(AudioDeviceProcessNode{std::move(process), std::move(audioProcessData)});
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
    typedGraph_.metadata(edgeHandle).set<ChannelEdge>(ChannelEdge{fromChannel, toChannel});
    return edgeHandle;
}

void AudioDeviceGraphBase::disconnect(ade::EdgeHandle edgeHandle)
{
    typedGraph_.metadata(edgeHandle->dstNode()).get<AudioDeviceProcessNode>().upstreamLatency = 0U;
    typedGraph_.erase(edgeHandle);
}
}