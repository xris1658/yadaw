#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/backend/AudioGraphBusConfiguration.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "audio/util/Summing.hpp"
#include "util/IntegerRange.hpp"

#include "common/DisableStreamBuffer.hpp"

#include <cstdio>
#include <execution>

std::uint64_t sampleIndex;

const YADAW::Audio::Device::AudioProcessData<float>* outputAudioProcessData = nullptr;

YADAW::Audio::Backend::AudioGraphBusConfiguration* busConfiguration = nullptr;
YADAW::Audio::Engine::AudioDeviceGraph graph;
decltype(graph.topologicalSort()) topo;

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
    for(auto& row: topo)
    {
        std::for_each(std::execution::par_unseq, row.begin(), row.end(),
            [](std::vector<YADAW::Audio::Engine::AudioDeviceGraph::AudioDeviceProcessNode>& nodes)
            {
                for(auto& node: nodes)
                {
                    node.doProcess();
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
            auto busConfig = YADAW::Audio::Backend::AudioGraphBusConfiguration(backend);
            auto bufferSize = backend.bufferSizeInFrames();
            std::printf("Buffer size: %d sample(s)\n", bufferSize);
            auto pool = YADAW::Audio::Util::AudioBufferPool::createPool<float>(bufferSize);
            busConfiguration = &busConfig;
            {
                std::vector<ade::NodeHandle> inputNodes;
                std::vector<std::vector<std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>>> inputs;
                std::vector<std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>> outputs;
                auto outputChannelCount = backend.channelCount(false, outputDeviceIndex);
                std::vector<YADAW::Audio::Backend::AudioGraphBusConfiguration::Bus*> inputDevices;
                auto& outputDevice = busConfig.getOutputBusAt(busConfig.appendBus(false, outputChannelCount))->get();
                inputs.reserve(outputChannelCount * inputDeviceCount);
                outputs.reserve(outputChannelCount);
                std::vector<YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>> inputProcessData;
                inputProcessData.reserve(inputDeviceCount);
                YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> outputProcessData;
                outputProcessData.setSingleBufferSize(bufferSize);
                outputProcessData.setInputGroupCount(1);
                outputProcessData.setInputCount(0, outputChannelCount);
                outputAudioProcessData = &outputProcessData.audioProcessData();
                YADAW::Audio::Util::Summing summing(
                    activatedInputDeviceCount,
                    YADAW::Audio::Base::ChannelGroupType::eCustomGroup,
                    outputChannelCount
                );
                YADAW::Audio::Util::AudioProcessDataPointerContainer<float> summingProcessData;
                summingProcessData.setSingleBufferSize(bufferSize);
                summingProcessData.setInputGroupCount(activatedInputDeviceCount);
                summingProcessData.setOutputGroupCount(1);
                summingProcessData.setOutputCount(0, outputChannelCount);
                FOR_RANGE0(i, outputChannelCount)
                {
                    outputs.emplace_back(
                        std::make_shared<YADAW::Audio::Util::AudioBufferPool::Buffer>(
                            pool->lend()
                        )
                    );
                    outputDevice.setChannel(i, YADAW::Audio::Device::Channel {outputDeviceIndex, i});
                    outputProcessData.setInputBuffer(0, i, outputs.back());
                    summingProcessData.setOutput(0, i, reinterpret_cast<float*>(outputs.back().get()->pointer()));
                }
                auto activatedInputDeviceIndex = 0;
                FOR_RANGE0(i, inputDeviceCount)
                {
                    auto& input = inputs.emplace_back(
                        decltype(inputs)::value_type(
                            outputChannelCount,
                            decltype(inputs)::value_type::value_type(nullptr)
                        )
                    );
                    inputDevices.emplace_back(nullptr);
                    auto& processData = inputProcessData.emplace_back();
                    if(backend.isDeviceInputActivated(i))
                    {
                        inputDevices.back() = &(busConfig.getInputBusAt(busConfig.appendBus(true, outputChannelCount))->get());
                        processData.setSingleBufferSize(bufferSize);
                        processData.setOutputGroupCount(1);
                        processData.setOutputCount(0, outputChannelCount);
                        summingProcessData.setInputCount(activatedInputDeviceIndex, outputChannelCount);
                        FOR_RANGE0(j, outputChannelCount)
                        {
                            input[j] = std::make_shared<YADAW::Audio::Util::AudioBufferPool::Buffer>(
                                pool->lend()
                            );
                            inputDevices.back()->setChannel(j, YADAW::Audio::Device::Channel {i, j});
                            processData.setOutputBuffer(0, j, input[j]);
                            summingProcessData.setInput(
                                activatedInputDeviceIndex, j,
                                reinterpret_cast<float*>(input[j].get()->pointer())
                            );
                        }
                        ++activatedInputDeviceIndex;
                        inputNodes.emplace_back(
                            graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*(inputDevices.back())),
                                processData.audioProcessData())
                        );
                    }
                }
                auto summingNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(summing),
                    summingProcessData.audioProcessData());
                auto outputNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(outputDevice),
                    outputProcessData.audioProcessData());
                FOR_RANGE0(i, inputNodes.size())
                {
                    graph.connect(inputNodes[i], summingNode, 0, i);
                }
                graph.connect(summingNode, outputNode, 0, 0);
                topo = graph.topologicalSort();
                backend.start(&callback);
                std::getchar();
                backend.stop();
            }
            backend.destroyAudioGraph();
        }
    }
}