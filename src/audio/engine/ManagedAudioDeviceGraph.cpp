#include "ManagedAudioDeviceGraph.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Util::AudioBufferPool;
ManagedAudioDeviceGraph::ManagedAudioDeviceGraph(
    std::uint32_t bufferSize,
    bool latencyCompensationEnabled):
    graph_(latencyCompensationEnabled),
    bufferSize_(bufferSize),
    pool_(AudioBufferPool::createPool<float>(bufferSize_)),
    dummyInput_(std::make_shared<AudioBufferPool::Buffer>(pool_->lend()))
{}

ade::NodeHandle ManagedAudioDeviceGraph::addNode(
    YADAW::Audio::Engine::AudioDeviceProcess process)
{
    YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> processData;
    processData.setSingleBufferSize(bufferSize_);
    auto device = process.device();
    auto inputGroupCount = device->audioInputGroupCount();
    processData.setInputGroupCount(inputGroupCount);
    FOR_RANGE0(i, inputGroupCount)
    {
        const auto& group = device->audioInputGroupAt(i)->get();
        auto channelCount = group.channelCount();
        processData.setInputCount(i, channelCount);
        FOR_RANGE0(j, channelCount)
        {
            // Multiple reads is not UB, so we just prepare one dummy buffer
            // for all disconnected audio input ports
            processData.setInputBuffer(i, j, dummyInput_);
        }
    }
    auto outputGroupCount = device->audioOutputGroupCount();
    processData.setOutputGroupCount(outputGroupCount);
    FOR_RANGE0(i, outputGroupCount)
    {
        const auto& group = device->audioOutputGroupAt(i)->get();
        auto channelCount = group.channelCount();
        processData.setOutputCount(i, channelCount);
        FOR_RANGE0(j, channelCount)
        {
            // Multiple writes is UB, so we prepare dummy buffers for each
            // disconnected audio output ports
            processData.setOutputBuffer(i, j,
                std::make_shared<AudioBufferPool::Buffer>(pool_->lend())
            );
        }
    }
    auto nodeHandle = graph_.addNode(
        std::move(process), processData.audioProcessData());
    processData_.emplace(nodeHandle, processData);
    return nodeHandle;
}

void ManagedAudioDeviceGraph::removeNode(ade::NodeHandle nodeHandle)
{
    graph_.removeNode(std::move(nodeHandle));
}

ade::EdgeHandle ManagedAudioDeviceGraph::connect(
    ade::NodeHandle from, ade::NodeHandle to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto ret = graph_.connect(from, to, fromChannel, toChannel);
    if(ret != nullptr)
    {
        auto& fromContainer = processData_.find(from)->second;
        auto& toContainer = processData_.find(to)->second;
        auto channelCount = toContainer.audioProcessData().inputCounts[toChannel];
        FOR_RANGE0(i, channelCount)
        {
            toContainer.setInputBuffer(toChannel, i,
                fromContainer.outputBuffer(fromChannel, i)
            );
        }
        graph_.getMetadata(to).processData = toContainer.audioProcessData();
    }
    return ret;
}
}