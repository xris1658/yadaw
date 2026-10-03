#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/PDC.hpp"
#include "audio/mixer/PluginHostBypass.hpp"
#include "audio/mixer/PolarityInverter.hpp"

#include "../common/SineWaveGenerator.hpp"

#include <thread>

int main()
{
    using namespace YADAW::Audio;
    using namespace YADAW::Audio::Engine;
    using namespace YADAW::Audio::Mixer;
    using namespace YADAW::Audio::Util;
    constexpr double sampleRate = 48000;
    constexpr std::uint32_t bufferSize = 8;
    AudioDeviceGraph<
        Extension::Buffer,
        Extension::UpstreamLatency,
        PDC::Extension
    > graph;
    auto& bufferExt = graph.getExtension<Extension::Buffer>();
    bufferExt.setBufferSize(bufferSize);
    SineWaveGenerator swg(YADAW::Audio::Base::ChannelGroupType::eMono);
    swg.setSampleRate(sampleRate);
    swg.setFrequency(8000);
    swg.startProcessing();
    PolarityInverter pi(YADAW::Audio::Base::ChannelGroupType::eMono);
    pi.setInverted(0x1);
    auto swgNode = graph.addNode(AudioDeviceProcess{swg});
    auto optPHB = PluginHostBypass::createIfNeeded(graph, AudioDeviceProcess{pi});
    auto& phb = *optPHB;
    std::vector<ade::EdgeHandle> edges;
    auto optInput = phb.inputAt(0); auto& inputs = std::get<1>(*optInput);
    edges.reserve(inputs.size());
    for(const auto& position: inputs)
    {
        edges.emplace_back(*graph.connect(swgNode, position.node, 0, position.index));
    }
    phb.initialize(sampleRate, bufferExt.bufferSize());
    std::atomic_flag running; running.test_and_set();
    auto processSeq = getProcessSequenceWithPrev(graph, bufferExt);
    std::thread audioThread(
        [&]() mutable
        {
            while(running.test(std::memory_order_acquire))
            {
                for(auto& vv: processSeq)
                {
                    for(auto& [first, second]: vv)
                    {
                        for(auto& [process, buffer]: first)
                        {
                            process.process(buffer.audioProcessData());
                        }
                    }
                }
            }
        }
    );
    while(true)
    {
        auto c = std::getchar();
        if(c == 'x')
        {
            break;
        }
        if(c == '0')
        {
            phb.bypassSwitcher().setValue(false);
        }
        if(c == '1')
        {
            phb.bypassSwitcher().setValue(true);
        }
    }
    std::vector<YADAW::Util::PMRUniquePtr<void>> dismissedData;
    NodeSet::dismissRecursively(phb, std::back_inserter(dismissedData));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    running.clear(std::memory_order_release);
    audioThread.join();
    optPHB.reset();
}