#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/mixer/PolarityInverter.hpp"
#include "audio/util/Summing.hpp"

#include "common/SineWaveGenerator.hpp"

int main()
{
    using namespace YADAW::Audio;
    using namespace YADAW::Audio::Engine;
    using namespace YADAW::Audio::Util;
    AudioDeviceGraph<
        Extension::Buffer,
        Extension::UpstreamLatency
    > graph;
    AudioDeviceGraphWithPDC graphWithPDC(
        graph, graph.getExtension<Extension::UpstreamLatency>()
    );
    auto& bufferExt = graph.getExtension<Extension::Buffer>();
    bufferExt.setBufferSize(8);
    auto mono = Base::ChannelGroupType::eMono;
    SineWaveGenerator sine(mono);
    sine.setFrequency(440.0);
    sine.setSampleRate(44100.0);
    sine.startProcessing();
    Mixer::PolarityInverter polarityInverter(mono);
    polarityInverter.setInverted(0b01);
    Summing summing(2, mono);
    auto sineNode = graph.addNode(AudioDeviceProcess(sine));
    auto polarityInverterNode = graph.addNode(AudioDeviceProcess(polarityInverter));
    auto summingNode = graphWithPDC.addNode(AudioDeviceProcess(summing));
    auto multiInput = static_cast<MultiInputDeviceWithPDC*>(
        graph.getNodeData(summingNode).process.device()
    );
    multiInput->setBufferSize(8);
    graph.connect(sineNode, summingNode, 0, 0);
    graph.connect(sineNode, polarityInverterNode, 0, 0);
    graph.connect(polarityInverterNode, summingNode, 0, 1);
    FOR_RANGE0(i, 8)
    {
        auto& sineContainer = bufferExt.getData(sineNode).container;
        sine.process(bufferExt.getData(sineNode).container.audioProcessData());
        polarityInverter.process(bufferExt.getData(polarityInverterNode).container.audioProcessData());
        auto& container = bufferExt.getData(summingNode).container;
        summing.process(container.audioProcessData());
        auto output = reinterpret_cast<float*>(container.outputBuffer(0, 0)->pointer());
        auto input = reinterpret_cast<float*>(sineContainer.outputBuffer(0, 0)->pointer());
        std::printf(" IN: ");
        FOR_RANGE0(j, 8)
        {
            std::printf("%f ", input[j]);
        }
        std::printf("\nOUT: ");
        FOR_RANGE0(j, 8)
        {
            std::printf("%f ", output[j]);
        }
        std::printf("\n------------------------------------------------------\n");
    }
    sine.stopProcessing();
}