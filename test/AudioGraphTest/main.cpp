#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/backend/AudioGraphBusConfiguration.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "audio/util/Summing.hpp"
#include "util/IntegerRange.hpp"

#include "common/DisableStreamBuffer.hpp"

#include <cstdio>
#include <execution>

std::uint64_t sampleIndex;

const YADAW::Audio::Device::AudioProcessData<float>* outputAudioProcessData = nullptr;

YADAW::Audio::Backend::AudioGraphBusConfiguration* busConfiguration = nullptr;
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
std::vector<std::vector<std::vector<YADAW::Audio::Engine::AudioDeviceGraphBase::NodeData>>> topoNodes;

void callback(int inputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    // FOR_RANGE0(i, outputAudioProcessData->inputGroupCount)
    // {
    //     FOR_RANGE0(j, outputAudioProcessData->inputCounts[i])
    //     {
    //         FOR_RANGE0(k, outputAudioProcessData->singleBufferSize)
    //         {
    //             float value = 0.5f * std::sin(2 * std::acos(-1.0f) * (sampleIndex + k) / (static_cast<float>(outputAudioProcessData->singleBufferSize)));
    //             outputAudioProcessData->inputs[i][j][k] = value;
    //         }
    //     }
    // }
    // sampleIndex += outputAudioProcessData->singleBufferSize;
    busConfiguration->setBuffers(inputs, outputs);
    for(auto& row: topoEntities)
    {
        std::for_each(std::execution::par_unseq, row.begin(), row.end(),
            [](std::vector<
                std::pair<
                    YADAW::Audio::Engine::AudioDeviceProcess,
                    YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>
                >
            >& nodes)
            {
                for(auto& [process, container]: nodes)
                {
                    process.process(container.audioProcessData());
                }
            }
        );
    }
}

int main()
{
    std::system("chcp 65001");
    disableStdOutBuffer();
    YADAW::Audio::Backend::AudioGraphBackend backend;
    if(backend.initialize())
    {
        auto outputDeviceCount = backend.audioOutputDeviceCount();
        std::printf("%u output devices: \n", outputDeviceCount);
        FOR_RANGE0(i, outputDeviceCount)
        {
            const auto& device = backend.audioOutputDeviceAt(i);
            std::printf("\t%u: %s (%s)\n", i + 1,
                device.name.toUtf8().data(),
                device.id.toUtf8().data()
            );
        }
        std::uint32_t outputDeviceIndex;
        do
        {
            std::scanf("%u", &outputDeviceIndex);
            if(outputDeviceIndex <= outputDeviceCount)
            {
                --outputDeviceIndex;
                std::getchar();
                break;
            }
        } while(true);
        if(backend.createAudioGraph(backend.audioOutputDeviceAt(outputDeviceIndex).id))
        {
            auto activatedInputDeviceCount = 0;
            auto inputDeviceCount = backend.audioInputDeviceCount();
            std::printf("%u input devices: \n", inputDeviceCount);
            do
            {
                FOR_RANGE0(i, inputDeviceCount)
                {
                    const auto& device = backend.audioInputDeviceAt(i);
                    std::printf("\t[%c] %u: %s (%s)\n",
                        backend.isDeviceInputActivated(i)? 'X': ' ',
                        i + 1,
                        device.name.toUtf8().data(),
                        device.id.toUtf8().data()
                    );
                }
                std::printf("Input device index to toggle device activation (input 0 to finish): ");
                std::uint32_t operation = 0;
                std::scanf("%d", &operation);
                if(operation > inputDeviceCount)
                {
                    continue;
                }
                if(operation == 0)
                {
                    std::getchar();
                    break;
                }
                backend.activateDeviceInput(operation - 1,
                    !backend.isDeviceInputActivated(operation - 1)
                );
                if(backend.isDeviceInputActivated(operation - 1))
                {
                    ++activatedInputDeviceCount;
                }
                else
                {
                    --activatedInputDeviceCount;
                }
            }
            while(true);
            {
                auto busConfig = YADAW::Audio::Backend::AudioGraphBusConfiguration(backend);
                auto bufferSize = backend.bufferSizeInFrames();
                std::printf("Buffer size: %d sample(s)\n", bufferSize);
                auto channelCount = backend.channelCount(false, outputDeviceIndex);
                busConfiguration = &busConfig;
                YADAW::Audio::Engine::AudioDeviceGraph<
                    YADAW::Audio::Engine::Extension::Buffer> graph;
                auto& bufferExtension = graph.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
                bufferExtension.setBufferSize(bufferSize);
                YADAW::Audio::Util::Summing summing(activatedInputDeviceCount,
                    YADAW::Audio::Base::ChannelGroupType::eCustomGroup, channelCount);
                auto& outputBus = busConfig.getOutputBusAt(busConfig.appendBus(false, YADAW::Audio::Base::ChannelGroupType::eCustomGroup, channelCount))->get();
                FOR_RANGE0(i, channelCount)
                {
                    outputBus.setChannel(i, YADAW::Audio::Device::Channel{outputDeviceIndex, i});
                }
                std::vector<YADAW::Audio::Backend::AudioGraphBusConfiguration::Bus*> inputBusses_;
                inputBusses_.reserve(activatedInputDeviceCount);
                FOR_RANGE0(i, backend.audioInputDeviceCount())
                {
                    if(backend.isDeviceInputActivated(i))
                    {
                        auto& inputBus = inputBusses_.emplace_back(
                            &(busConfig.getInputBusAt(busConfig.appendBus(true, YADAW::Audio::Base::ChannelGroupType::eCustomGroup, channelCount))->get())
                        );
                        FOR_RANGE0(j, channelCount)
                        {
                            inputBus->setChannel(j, YADAW::Audio::Device::Channel{i, j});
                        }
                    }
                }
                std::vector<ade::NodeHandle> inputNodes;
                ade::NodeHandle summingNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(summing));
                ade::NodeHandle outputNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(outputBus));
                graph.connect(summingNode, outputNode, 0U, 0U);
                FOR_RANGE0(i, activatedInputDeviceCount)
                {
                    auto inputNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*inputBusses_[i]));
                    graph.connect(inputNode, summingNode, 0U, i);
                }
                auto topo = graph.topologicalSort();
                topoEntities.reserve(topo.size());
                for(auto& row: topo)
                {
                    auto& rowEntities = topoEntities.emplace_back();
                    rowEntities.reserve(row.size());
                    for(auto& cell: row)
                    {
                        auto& cellEntities = rowEntities.emplace_back();
                        cellEntities.reserve(cell.size());
                        for(auto& node: cell)
                        {
                            cellEntities.emplace_back(
                                std::make_pair(
                                    static_cast<const YADAW::Audio::Engine::AudioDeviceProcess&>(graph.getNodeData(node).process),
                                    graph.getExtensionData<YADAW::Audio::Engine::Extension::Buffer>(node).container
                                )
                            );
                        }
                    }
                }
                backend.start(&callback);
                std::getchar();
                backend.stop();
            }
            backend.destroyAudioGraph();
        }
    }
}