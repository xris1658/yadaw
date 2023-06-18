#include "AudioDeviceGraph.hpp"

namespace YADAW::Audio::Engine
{

AudioDeviceGraph::AudioDeviceGraph():
    graph_(),
    typedGraph_(graph_) {}

ade::NodeHandle AudioDeviceGraph::addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto nodeHandle = typedGraph_.createNode();
    typedGraph_.metadata(nodeHandle).set(AudioDeviceProcessNode{std::move(process), std::move(audioProcessData)});
    return nodeHandle;
}

void AudioDeviceGraph::removeNode(ade::NodeHandle nodeHandle)
{
    typedGraph_.metadata(nodeHandle).erase<AudioDeviceProcessNode>();
    typedGraph_.erase(nodeHandle);
}
}