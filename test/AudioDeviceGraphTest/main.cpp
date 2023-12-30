#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/util/SampleDelay.hpp"
#include "audio/util/Summing.hpp"
#include "util/IntegerRange.hpp"

#include "common/SineWaveGenerator.hpp"

#include <iostream>

int main()
{
    using namespace YADAW::Audio::Engine::Extension;
    std::vector<YADAW::Audio::Util::SampleDelay> sampleDelay;
    const auto nodeCount = 8;
    sampleDelay.reserve(nodeCount);
    std::vector<SineWaveGenerator> generators;
    generators.reserve(nodeCount);
    std::vector<ade::NodeHandle> generatorNodes;
    std::vector<ade::NodeHandle> sampleDelayNodes;
    sampleDelayNodes.reserve(nodeCount);
    std::vector<ade::EdgeHandle> edges;
    edges.reserve(nodeCount - 1);
    YADAW::Audio::Util::AudioChannelGroup channelGroup;
    channelGroup.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eStereo);
    YADAW::Audio::Util::Summing summing(nodeCount, channelGroup.type());
    {
        YADAW::Audio::Engine::AudioDeviceGraph<Buffer, UpstreamLatency> graph;
        auto& bufferExt = graph.getExtension<Buffer>();
        bufferExt.setBufferSize(32);
        FOR_RANGE0(i, nodeCount)
        {
            auto& generator = generators.emplace_back(channelGroup.type());
            generator.setFrequency(440 * (i % 2 == 0? -1: +1));
            generator.setSampleRate(44100);
            sampleDelay.emplace_back((i * 5) % nodeCount + 1, channelGroup);
            sampleDelayNodes.emplace_back(
                graph.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(sampleDelay[i])
                )
            );
            generatorNodes.emplace_back(
                graph.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(generator)
                )
            );
        }
        auto& upstreamLatencyExt = graph.getExtension<UpstreamLatency>();
        for(int i = nodeCount - 2; i >= 0; --i)
        {
            edges.emplace(edges.begin(), *graph.connect(sampleDelayNodes[i], sampleDelayNodes[i + 1], 0, 0));
            FOR_RANGE0(j, sampleDelayNodes.size())
            {
                const auto& node = sampleDelayNodes[j];
                std::cout
                    << upstreamLatencyExt.getMaxUpstreamLatency(node)
                    << ", "
                    << graph.getNodeData(node).process.device()->latencyInSamples()
                    << ' '
                    << ((j >= i && j != sampleDelayNodes.size() - 1)? '|': ' ')
                    << '\n';
            }
            std::cout << "----------------\n";
        }
        std::cout << "Disconnecting\n";
        FOR_RANGE0(i, edges.size())
        {
            const auto& edge = edges[i];
            graph.disconnect(edge);
            FOR_RANGE0(j, sampleDelayNodes.size())
            {
                const auto& node = sampleDelayNodes[j];
                std::cout
                    << upstreamLatencyExt.getMaxUpstreamLatency(node)
                    << ", "
                    << graph.getNodeData(node).process.device()->latencyInSamples()
                    << ' '
                    << ((j > i && j != sampleDelayNodes.size() - 1)? '|': ' ')
                    << '\n';
            }
            std::cout << "----------------\n";
        }
        for(const auto& node: sampleDelayNodes)
        {
            std::cout
                << upstreamLatencyExt.getMaxUpstreamLatency(node)
                << ", "
                << graph.getNodeData(node).process.device()->latencyInSamples()
                << '\n';
        }
        YADAW::Audio::Engine::AudioDeviceGraphWithPDC graphWithPDC(graph);
        auto summingNode = graphWithPDC.addNode(YADAW::Audio::Engine::AudioDeviceProcess{summing});
        edges.clear();
        edges.reserve(nodeCount);
        FOR_RANGE0(i, nodeCount)
        {
            graph.connect(generatorNodes[i], sampleDelayNodes[i], 0, 0);
            edges.emplace_back(*(graph.connect(sampleDelayNodes[i], summingNode, 0, i)));
            std::cout << upstreamLatencyExt.getMaxUpstreamLatency(summingNode) << '\n';
        }
        const auto& summingAudioProcessData = graph.getExtensionData<Buffer>(summingNode).container.audioProcessData();
        FOR_RANGE0(i, 10)
        {
            for(auto& generatorNode: generatorNodes)
            {
                graph.getNodeData(generatorNode).process.process(
                    graph.getExtensionData<Buffer>(generatorNode).container.audioProcessData()
                );
            }
            for(auto& sampleDelayNode: sampleDelayNodes)
            {
                graph.getNodeData(sampleDelayNode).process.process(
                    graph.getExtensionData<Buffer>(sampleDelayNode).container.audioProcessData()
                );
            }
            graph.getNodeData(summingNode).process.process(summingAudioProcessData);
            FOR_RANGE0(i2, summingAudioProcessData.outputGroupCount)
            {
                FOR_RANGE0(i3, summingAudioProcessData.outputCounts[i2])
                {
                    auto buffer = summingAudioProcessData.outputs[i2][i3];
                    auto nulled = std::all_of(
                        buffer, buffer + summingAudioProcessData.singleBufferSize,
                        [](float data) { return data == 0; }
                    );
                    assert(nulled);
                }
            }
        }
        FOR_RANGE0(i, nodeCount)
        {
            graph.disconnect(edges[(i + 1) * 3 % nodeCount]);
            std::cout << upstreamLatencyExt.getMaxUpstreamLatency(summingNode) << '\n';
        }
    }
}