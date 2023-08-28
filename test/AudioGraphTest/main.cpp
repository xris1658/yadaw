#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/backend/AudioGraphBusConfiguration.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/util/AudioBufferPool.hpp"
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
    FOR_RANGE0(i, outputAudioProcessData->inputGroupCount)
    {
        FOR_RANGE0(j, outputAudioProcessData->inputCounts[i])
        {
            FOR_RANGE0(k, outputAudioProcessData->singleBufferSize)
            {
                float value = 0.5f * std::sin(2 * std::acos(-1.0f) * (sampleIndex + k) / (static_cast<float>(outputAudioProcessData->singleBufferSize)));
                outputAudioProcessData->inputs[i][j][k] = value;
            }
        }
    }
    sampleIndex += outputAudioProcessData->singleBufferSize;
    busConfiguration->setBuffers(inputs, outputs);
    for(auto& row: topo)
    {
        std::for_each(std::execution::par_unseq, row.begin(), row.end(),
            [](std::vector<YADAW::Audio::Engine::AudioDeviceGraphBase::AudioDeviceProcessNode>& nodes)
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
                device.name.toLocal8Bit().data(),
                device.id.toLocal8Bit().data()
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
            auto busConfig = YADAW::Audio::Backend::AudioGraphBusConfiguration(backend);
            auto bufferSize = backend.bufferSizeInFrames();
            std::printf("Buffer size: %d sample(s)\n", bufferSize);
            auto pool = YADAW::Audio::Util::AudioBufferPool::createPool(bufferSize * sizeof(float));
            busConfiguration = &busConfig;
            {
                std::vector<std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>> outputs;
                auto outputChannelCount = backend.channelCount(false, outputDeviceIndex);
                auto& outputDevice = busConfig.getOutputBusAt(busConfig.appendBus(false, outputChannelCount))->get();
                outputs.reserve(outputChannelCount);
                YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> outputProcessData;
                outputProcessData.setSingleBufferSize(bufferSize);
                outputProcessData.setInputGroupCount(1);
                outputProcessData.setInputCount(0, outputChannelCount);
                outputAudioProcessData = &outputProcessData.audioProcessData();
                FOR_RANGE0(i, outputChannelCount)
                {
                    outputs.emplace_back(
                        std::make_shared<YADAW::Audio::Util::AudioBufferPool::Buffer>(
                            pool->lend()
                        )
                    );
                    outputDevice.setChannel(i, YADAW::Audio::Device::Channel {outputDeviceIndex, i});
                    outputProcessData.setInputBuffer(0, i, outputs.back());
                }
                auto node = graph.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(outputDevice),
                    outputProcessData.audioProcessData()
                );
                topo = graph.topologicalSort();
                backend.start(&callback);
                std::getchar();
                backend.stop();
            }
            backend.destroyAudioGraph();
        }
    }
}