#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/util/SampleDelay.hpp"
#include "audio/util/Summing.hpp"
#include "util/IntegerRange.hpp"
#include "util/Util.hpp"

#include "common/DisableStreamBuffer.hpp"
#include "common/SineWaveGenerator.hpp"

#include <cstdio>

int main()
{
    using namespace YADAW::Audio::Engine::Extension;
    disableStdOutBuffer();
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
    channelGroup.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eMono);
    YADAW::Audio::Util::Summing summing(nodeCount, channelGroup.type());
    {
        YADAW::Audio::Engine::AudioDeviceGraph<Buffer, UpstreamLatency> graph;
        auto bufferSize = 32;
        auto& bufferExt = graph.getExtension<Buffer>();
        bufferExt.setBufferSize(bufferSize);
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
                std::printf("%d, %d %c\n",
                    upstreamLatencyExt.getMaxUpstreamLatency(node),
                    graph.getNodeData(node).process.device()->latencyInSamples(),
                    (j >= i && j != sampleDelayNodes.size() - 1)? '|': ' '
                );
            }
            std::printf("----------------\n");
        }
        std::printf("Disconnecting\n");
        FOR_RANGE0(i, edges.size())
        {
            const auto& edge = edges[i];
            graph.disconnect(edge);
            FOR_RANGE0(j, sampleDelayNodes.size())
            {
                const auto& node = sampleDelayNodes[j];
                std::printf("%d, %d %c\n",
                    upstreamLatencyExt.getMaxUpstreamLatency(node),
                    graph.getNodeData(node).process.device()->latencyInSamples(),
                    (j > i && j != sampleDelayNodes.size() - 1)? '|': ' '
                );
            }
            std::printf("----------------\n");
        }
        for(const auto& node: sampleDelayNodes)
        {
            std::printf("%u, %u\n",
                upstreamLatencyExt.getMaxUpstreamLatency(node),
                graph.getNodeData(node).process.device()->latencyInSamples()
            );
        }
        YADAW::Audio::Engine::AudioDeviceGraphWithPDC graphWithPDC(graph);
        auto summingNode = graphWithPDC.addNode(YADAW::Audio::Engine::AudioDeviceProcess{summing});
        edges.clear();
        edges.reserve(nodeCount);
        FOR_RANGE0(i, nodeCount)
        {
            graph.connect(generatorNodes[i], sampleDelayNodes[i], 0, 0);
            edges.emplace_back(*(graph.connect(sampleDelayNodes[i], summingNode, 0, i)));
            std::printf("%u\n", upstreamLatencyExt.getMaxUpstreamLatency(summingNode));
        }
        const auto& summingAudioProcessData = graph.getExtensionData<Buffer>(summingNode).container.audioProcessData();
        for(auto& device: sampleDelay)
        {
            device.startProcessing();
        }
        for(auto& device: generators)
        {
            device.startProcessing();
        }
        FOR_RANGE0(z, 10)
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
            FOR_RANGE0(i, summingAudioProcessData.inputGroupCount)
            {
                FOR_RANGE0(j, summingAudioProcessData.singleBufferSize)
                {
                    auto data = summingAudioProcessData.inputs[i][0][j];
                    if(!std::signbit(data))
                    {
                        std::printf("+");
                    }
                    std::printf("%f\t", data);
                }
                std::printf("\n");
            }
            std::printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
            graph.getNodeData(summingNode).process.process(summingAudioProcessData);
            FOR_RANGE0(i, summingAudioProcessData.inputGroupCount)
            {
                FOR_RANGE0(j, summingAudioProcessData.singleBufferSize)
                {
                    auto data = summingAudioProcessData.inputs[i][0][j];
                    if(!std::signbit(data))
                    {
                        std::printf("+");
                    }
                    std::printf("%f\t", data);
                }
                std::printf("\n");
            }
            auto outputCount = summingAudioProcessData.outputCounts[0];
            auto outputs = summingAudioProcessData.outputs[0];
            FOR_RANGE0(i, outputCount)
            {
                auto buffer = outputs[i];;
                auto nulled = std::all_of(
                    buffer, buffer + summingAudioProcessData.singleBufferSize,
                    [](float data) { return data == 0; }
                );
                assert(nulled);
                std::printf("Process #%d passed\n", z + 1);
            }
            // auto updateLatencyIndex = std::rand() % nodeCount;
            // auto& device = sampleDelay[updateLatencyIndex];
            // device.stopProcessing();
            // auto newDelay = std::rand() % 32;
            // std::printf("New delay of device %d: %d\n", updateLatencyIndex, newDelay);
            // device.setDelay(newDelay);
            // upstreamLatencyExt.onLatencyOfNodeUpdated(sampleDelayNodes[updateLatencyIndex]);
            // device.startProcessing();
        }
        for(auto& device: sampleDelay)
        {
            device.stopProcessing();
        }
        for(auto& device: generators)
        {
            device.stopProcessing();
        }
        FOR_RANGE0(i, nodeCount)
        {
            graph.disconnect(edges[(i + 1) * 3 % nodeCount]);
            std::printf("%u\n", upstreamLatencyExt.getMaxUpstreamLatency(summingNode));
        }
    }
}