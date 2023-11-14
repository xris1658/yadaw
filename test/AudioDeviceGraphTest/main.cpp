#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/util/SampleDelay.hpp"
#include "audio/util/Summing.hpp"
#include "util/IntegerRange.hpp"

#include <iostream>

int main()
{
    std::vector<YADAW::Audio::Util::SampleDelay> sampleDelay;
    const auto nodeCount = 8;
    sampleDelay.reserve(nodeCount);
    std::vector<ade::NodeHandle> nodes;
    nodes.reserve(nodeCount);
    std::vector<ade::EdgeHandle> edges;
    edges.reserve(nodeCount - 1);
    YADAW::Audio::Util::AudioChannelGroup channelGroup;
    channelGroup.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eStereo);
    YADAW::Audio::Util::Summing summing(nodeCount, channelGroup.type());
    {
        YADAW::Audio::Engine::AudioDeviceGraph<
            YADAW::Audio::Engine::Extension::UpstreamLatency> graph;
        FOR_RANGE0(i, nodeCount)
        {
            sampleDelay.emplace_back((i * 5) % nodeCount + 1, channelGroup);
            nodes.emplace_back(
                graph.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(sampleDelay[i])
                )
            );
        }
        auto& upstreamLatencyExt = graph.getExtension<YADAW::Audio::Engine::Extension::UpstreamLatency>();
        for(int i = nodeCount - 2; i >= 0; --i)
        {
            edges.emplace(edges.begin(), *graph.connect(nodes[i], nodes[i + 1], 0, 0));
            for(const auto& node: nodes)
            {
                std::cout
                    << upstreamLatencyExt.getUpstreamLatency(node)
                    << ", "
                    << graph.getNodeData(node).process.device()->latencyInSamples()
                    << '\n';
            }
            std::cout << "----------------\n";
        }
        std::cout << "Disconnecting\n";
        for(const auto& edge: edges)
        {
            graph.disconnect(edge);
            for(const auto& node: nodes)
            {
                std::cout
                    << upstreamLatencyExt.getUpstreamLatency(node)
                    << ", "
                    << graph.getNodeData(node).process.device()->latencyInSamples()
                    << '\n';
            }
            std::cout << "----------------\n";
        }
        for(const auto& node: nodes)
        {
            std::cout
                << upstreamLatencyExt.getUpstreamLatency(node)
                << ", "
                << graph.getNodeData(node).process.device()->latencyInSamples()
                << '\n';
        }
        auto summingNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess{summing});
        edges.clear();
        edges.reserve(nodeCount);
        FOR_RANGE0(i, nodeCount)
        {
            edges.emplace_back(*(graph.connect(nodes[i], summingNode, 0, i)));
            std::cout << upstreamLatencyExt.getUpstreamLatency(summingNode) << '\n';
        }
        FOR_RANGE0(i, nodeCount)
        {
            graph.disconnect(edges[(i + 1) * 3 % nodeCount]);
            std::cout << upstreamLatencyExt.getUpstreamLatency(summingNode) << '\n';
        }
    }
}