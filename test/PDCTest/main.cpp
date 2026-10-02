#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/mixer/PolarityInverter.hpp"
#include "audio/util/Summing.hpp"
#include "audio/engine/PDC.hpp"

#include "common/SineWaveGenerator.hpp"

class SampleDelayWithLatency: public YADAW::Audio::Util::SampleDelay
{
public:
    SampleDelayWithLatency(std::uint32_t delay,
        const YADAW::Audio::Device::IAudioChannelGroup& channelGroup
    ): SampleDelay(delay, channelGroup) {}
    std::uint32_t latencyInSamples() const override { return delay(); }
};

int main()
{
    using namespace YADAW::Audio;
    using namespace YADAW::Audio::Engine;
    using namespace YADAW::Audio::Util;
    AudioDeviceGraph<
        Extension::Buffer,
        Extension::UpstreamLatency,
        PDC::Extension
    > graph;
    auto& bufferExt = graph.getExtension<Extension::Buffer>();
    bufferExt.setBufferSize(8);
    auto mono = Base::ChannelGroupType::eMono;
    SineWaveGenerator sine(mono);
    sine.setFrequency(440.0);
    sine.setSampleRate(44100.0);
    Mixer::PolarityInverter polarityInverter(mono);
    polarityInverter.setInverted(0b1);
    SampleDelayWithLatency sampleDelay1(0, sine.audioOutputGroupAt(0)->get());
    SampleDelayWithLatency sampleDelay2(0, sine.audioOutputGroupAt(0)->get());
    Summing summing(2, mono);
    auto sineNode = graph.addNode(AudioDeviceProcess(sine));
    auto polarityInverterNode = graph.addNode(AudioDeviceProcess(polarityInverter));
    auto sampleDelayNode1 = graph.addNode(AudioDeviceProcess(sampleDelay1));
    auto sampleDelayNode2 = graph.addNode(AudioDeviceProcess(sampleDelay2));
    auto summingPDC = PDC::createIfNeeded(graph, AudioDeviceProcess(summing));
    auto summingNode = summingPDC->deviceNode();
    graph.connect(sineNode, sampleDelayNode1, 0, 0);
    auto summingInput1 = std::get<std::vector<NodeSet::NodePosition>>(*summingPDC->inputAt(0));
    auto summingInput2 = std::get<std::vector<NodeSet::NodePosition>>(*summingPDC->inputAt(1));
    graph.connect(sineNode, polarityInverterNode, 0, 0);
    graph.connect(polarityInverterNode, sampleDelayNode2, 0, 0);
    for(const auto& summingInput: summingInput1)
    {
        graph.connect(sampleDelayNode1, summingInput.node, 0, summingInput.index);
    }
    for(const auto& summingInput: summingInput2)
    {
        graph.connect(sampleDelayNode1, summingInput.node, 0, summingInput.index);
    }
    sine.startProcessing();
    summingPDC->startProcessing();
    FOR_RANGE0(i, 10)
    {
        std::printf("Round %d:\n", i + 1);
        sampleDelay1.setDelay(i == 9? 0: i);
        sampleDelay2.setDelay(i == 9? 0: 8 - i);
        graph.getExtension<Extension::UpstreamLatency>().onLatencyOfNodeUpdated(sampleDelayNode1);
        graph.getExtension<Extension::UpstreamLatency>().onLatencyOfNodeUpdated(sampleDelayNode2);
        sampleDelay1.startProcessing();
        sampleDelay2.startProcessing();
        std::printf("Delay 1: %u %u\n", sampleDelay1.delay(), summingPDC->pdcAt(0)->get().delay());
        std::printf("Delay 2: %u %u\n", sampleDelay2.delay(), summingPDC->pdcAt(1)->get().delay());
        FOR_RANGE0(j, 2)
        {
            auto& sineContainer = bufferExt.getNodeData(sineNode).container;
            sine.process(bufferExt.getNodeData(sineNode).container.audioProcessData());
            sampleDelay1.process(bufferExt.getNodeData(sampleDelayNode1).container.audioProcessData());
            polarityInverter.process(bufferExt.getNodeData(polarityInverterNode).container.audioProcessData());
            sampleDelay2.process(bufferExt.getNodeData(sampleDelayNode2).container.audioProcessData());
            FOR_RANGE0(k, summing.audioInputGroupCount())
            {
                summingPDC->pdcAt(k)->get().process(bufferExt.getNodeData(*summingPDC->pdcNodeAt(k)).container.audioProcessData());
            }
            auto& container = bufferExt.getNodeData(summingNode).container;
            summing.process(container.audioProcessData());
            auto output = reinterpret_cast<float*>(container.outputBuffer(0, 0)->pointer());
            auto input  = reinterpret_cast<float*>(sineContainer.outputBuffer(0, 0)->pointer());
            auto input1 = reinterpret_cast<float*>(container.inputBuffer(0, 0)->pointer());
            auto input2 = reinterpret_cast<float*>(container.inputBuffer(1, 0)->pointer());
            std::printf("IN0: ");
            FOR_RANGE0(k, 8)
            {
                std::printf("%+f, ", input[k]);
            }
            std::printf("\n");
            std::printf("IN1: ");
            FOR_RANGE0(k, 8)
            {
                std::printf("%+f, ", input1[k]);
            }
            std::printf("\n");
            std::printf("IN2: ");
            FOR_RANGE0(k, 8)
            {
                std::printf("%+f, ", input2[k]);
            }
            std::printf("\n");
            std::printf("OUT: ");
            FOR_RANGE0(k, 8)
            {
                std::printf("%+f, ", output[k]);
            }
            std::printf("%s\n", std::all_of(output, output + 8, [](float v) { return v == 0.0; })? "true": "false");
            std::printf("\n------------------------------------------------------\n");
        }
        sampleDelay1.stopProcessing();
        sampleDelay2.stopProcessing();
    }
    summingPDC->stopProcessing();
    sine.stopProcessing();
    auto dismissed = summingPDC->dismiss();
    graph.clear();
}