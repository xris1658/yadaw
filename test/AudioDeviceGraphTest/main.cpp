#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/util/SampleDelay.hpp"
#include "util/IntegerRange.hpp"

#include <iostream>

int main()
{
    YADAW::Audio::Engine::AudioDeviceGraph<
        YADAW::Audio::Engine::Extension::UpstreamLatency> graph(512);
    std::vector<YADAW::Audio::Util::SampleDelay> sampleDelay;
    const auto nodeCount = 8;
    sampleDelay.reserve(nodeCount);
    std::vector<ade::NodeHandle> nodes;
    nodes.reserve(nodeCount);
    YADAW::Audio::Util::AudioChannelGroup channelGroup;
    channelGroup.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eStereo);
    FOR_RANGE0(i, nodeCount)
    {
        sampleDelay.emplace_back((i * 5) % nodeCount + 1, channelGroup);
        nodes.emplace_back(
            graph.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(sampleDelay[i])
            )
        );
    }
    FOR_RANGE0(i, nodeCount - 1)
    {
        graph.connect(nodes[i], nodes[i + 1], 0, 0);
    }
    auto& upstreamLatencyExt = graph.getExtension<YADAW::Audio::Engine::Extension::UpstreamLatency>();
    for(const auto& node: nodes)
    {
        std::cout
            << upstreamLatencyExt.getUpstreamLatency(node)
            << ", "
            << graph.getNodeData(node).process.device()->latencyInSamples()
            << '\n';
    }
}