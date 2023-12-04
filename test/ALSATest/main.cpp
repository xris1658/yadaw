#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSABusConfiguration.hpp"
#include "audio/engine/AudioDeviceGraphBase.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "midi/MIDIOutputDevice.hpp"
#include "native/linux/ALSADeviceEnumerator.hpp"
#include "util/IntegerRange.hpp"
#include "util/Endian.hpp"
#include "common/DisableStreamBuffer.hpp"
#include "common/SineWaveGenerator.hpp"

#include <cmath>
#include <cstdio>
#include <execution>
#include <thread>

YADAW::Audio::Backend::ALSABusConfiguration* alsaBusConfiguration = nullptr;

std::vector<std::vector<std::vector<ade::NodeHandle>>> topologicalSortResult;
std::vector<std::vector<std::vector<YADAW::Audio::Engine::AudioDeviceGraphBase::NodeData>>> topoNodes;
std::vector<
    std::vector<
        std::vector<
            std::pair<
                YADAW::Audio::Engine::AudioDeviceProcess,
                YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
            >
        >
    >
> topoEntities;

std::uint64_t totalFrameCount = 0;

using YADAW::Audio::Backend::ALSABackend;

void callback(const ALSABackend* backend,
    std::uint32_t inputCount, ALSABackend::AudioBuffer* inputBuffers,
    std::uint32_t outputCount, ALSABackend::AudioBuffer* outputBuffers);

int main(int argc, char** argv)
{
    disableStdOutBuffer();
    auto audioInputCount = YADAW::Audio::Backend::ALSABackend::audioInputDeviceCount();
    std::printf("Audio input device count: %u", audioInputCount);
    for(decltype(audioInputCount) i = 0; i < audioInputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioInputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto audioOutputCount = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceCount();
    std::printf("\n\nAudio output device count: %u", audioOutputCount);
    for(decltype(audioOutputCount) i = 0; i < audioOutputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto midiInputCount = YADAW::Native::ALSADeviceEnumerator::midiInputDevices().size();
    std::printf("\n\nMIDI input device count: %zu", midiInputCount);
    for(decltype(midiInputCount) i = 0; i < midiInputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiInputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    auto midiOutputCount = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices().size();
    std::printf("\n\nMIDI output device count: %zu", midiOutputCount);
    for(decltype(midiOutputCount) i = 0; i < midiOutputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    std::printf("\n\n");
    YADAW::Audio::Backend::ALSABackend backend;
    auto frameCount = 512U;
    backend.initialize(44100, frameCount);
    YADAW::Audio::Backend::ALSADeviceSelector selector(0, 0);
    auto inputIndex = YADAW::Audio::Backend::findDeviceBySelector(backend, true, selector);
    auto inputChannelCount = 0U;
    auto outputIndex = findDeviceBySelector(backend, false, selector);
    auto outputChannelCount = 0U;
    if(inputIndex.has_value())
    {
        auto activateInputResult = backend.setAudioDeviceActivated(true, *inputIndex, true);
        if(activateInputResult == decltype(activateInputResult)::Failed)
        {
            std::printf("Activate default input failed!\n");
        }
        else
        {
            inputChannelCount = backend.channelCount(true, *inputIndex);
        }
    }
    if(outputIndex.has_value())
    {
        auto activateOutputResult = backend.setAudioDeviceActivated(false, *outputIndex, true);
        if(activateOutputResult == decltype(activateOutputResult)::Failed)
        {
            std::printf("Activate default output failed!\n");
        }
        else
        {
            outputChannelCount = backend.channelCount(false, *outputIndex);
        }
    }
    YADAW::Audio::Backend::ALSABusConfiguration busConfiguration(backend);
    alsaBusConfiguration = &busConfiguration;
    auto& inputBus = busConfiguration.getInputBusAt(busConfiguration.appendBus(true, inputChannelCount))->get();
    auto& outputBus = busConfiguration.getOutputBusAt(busConfiguration.appendBus(false, outputChannelCount))->get();
    auto sineWaveGenerator = SineWaveGenerator(YADAW::Audio::Base::ChannelGroupType::eCustomGroup, 2);
    sineWaveGenerator.setFrequency(440);
    sineWaveGenerator.setSampleRate(backend.sampleRate());
    YADAW::Audio::Engine::AudioDeviceGraph<
        YADAW::Audio::Engine::Extension::Buffer> graph;
    auto& bufferExt = graph.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
    bufferExt.setBufferSize(frameCount);
    FOR_RANGE0(i, inputChannelCount)
    {
        inputBus.setChannel(i, {*inputIndex, i});
    }
    FOR_RANGE0(i, outputChannelCount)
    {
        outputBus.setChannel(i, {*outputIndex, i});
    }
    auto inputNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(inputBus));
    auto outputNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(outputBus));
    auto swgNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(sineWaveGenerator));
    // graph.connect(inputNode, outputNode, 0, 0);
    graph.connect(swgNode, outputNode, 0, 0);
    topologicalSortResult = graph.topologicalSort();
    topoNodes.reserve(topologicalSortResult.size());
    topoEntities.reserve(topologicalSortResult.size());
    for(auto& row: topologicalSortResult)
    {
        auto& dataRow = topoNodes.emplace_back();
        auto& entityRow = topoEntities.emplace_back();
        dataRow.reserve(row.size());
        entityRow.reserve(row.size());
        for(auto& cell: row)
        {
            auto& cellRow = dataRow.emplace_back();
            auto& entityCell = entityRow.emplace_back();
            cellRow.reserve(cell.size());
            entityCell.reserve(cell.size());
            for(auto& unit: cell)
            {
                cellRow.emplace_back(graph.getNodeData(unit));
                entityCell.emplace_back(
                    std::make_pair(
                        static_cast<const YADAW::Audio::Engine::AudioDeviceProcess&>(graph.getNodeData(unit).process),
                        graph.getExtensionData<YADAW::Audio::Engine::Extension::Buffer>(unit).container
                    )
                );
            }
        }
    }
    backend.start(callback);
    getchar();
    backend.stop();
}

void callback(const ALSABackend* backend,
    std::uint32_t inputCount, ALSABackend::AudioBuffer* inputBuffers,
    std::uint32_t outputCount, ALSABackend::AudioBuffer* outputBuffers)
{
    alsaBusConfiguration->setBuffers(inputBuffers, outputBuffers);
    for(auto& row: topoEntities)
    {
        std::for_each(std::execution::par_unseq, row.begin(), row.end(),
            [](std::vector<
                std::pair<
                    YADAW::Audio::Engine::AudioDeviceProcess,
                    YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
                >
            >& cell)
            {
                for(auto& [process, container]: cell)
                {
                    process.process(container.audioProcessData());
                }
            }
        );
    }
}
