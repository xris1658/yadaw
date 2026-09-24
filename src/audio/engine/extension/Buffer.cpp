#include "Buffer.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Engine::Extension
{
using YADAW::Audio::Util::AudioBufferPool;

Buffer::Buffer(AudioDeviceGraphBase& graph):
    graph_(graph)
{}

void Buffer::onNodeAdded(const ade::NodeHandle& nodeHandle)
{
    auto& device = *(graph_.getNodeData(nodeHandle).process.device());
    auto& container = getNodeData_(graph_, nodeHandle).container;
    container.setSingleBufferSize(bufferSize());
    const auto inputGroupCount = device.audioInputGroupCount();
    container.setInputGroupCount(inputGroupCount);
    FOR_RANGE0(i, inputGroupCount)
    {
        const auto inputCount = device.audioInputGroupAt(i)->get().channelCount();
        container.setInputCount(i, inputCount);
        FOR_RANGE0(j, inputCount)
        {
            container.setInputBuffer(i, j, getGraphData_(graph_).dummyInput);
        }
    }
    const auto outputGroupCount = device.audioOutputGroupCount();
    container.setOutputGroupCount(outputGroupCount);
    FOR_RANGE0(i, outputGroupCount)
    {
        const auto outputCount = device.audioOutputGroupAt(i)->get().channelCount();
        container.setOutputCount(i, outputCount);
        FOR_RANGE0(j, outputCount)
        {
            container.setOutputBuffer(
                i, j, std::make_shared<YADAW::Audio::Util::AudioBufferPool::Buffer>(
                    getGraphData_(graph_).pool->lend()
                )
            );
        }
    }
}

void Buffer::onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle)
{}

void Buffer::onConnected(const ade::EdgeHandle& edgeHandle)
{
    const auto& fromNode = edgeHandle->srcNode();
    auto fromChannel = graph_.getEdgeData(edgeHandle).fromChannel;
    auto& fromNodeContainer = getNodeData_(graph_, fromNode).container;
    const auto& toNode = edgeHandle->dstNode();
    auto toChannel = graph_.getEdgeData(edgeHandle).toChannel;
    auto& toNodeContainer = getNodeData_(graph_, toNode).container;
    auto channelCount = toNodeContainer.audioProcessData().inputCounts[toChannel];
    FOR_RANGE0(i, channelCount)
    {
        toNodeContainer.setInputBuffer(toChannel, i,
            fromNodeContainer.outputBuffer(fromChannel, i));
    }
}

void Buffer::onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle)
{
    const auto& toNode = edgeHandle->dstNode();
    auto toChannel = graph_.getEdgeData(edgeHandle).toChannel;
    auto& toNodeContainer = getNodeData_(graph_, toNode).container;
    auto channelCount = toNodeContainer.audioProcessData().inputCounts[toChannel];
    FOR_RANGE0(i, channelCount)
    {
        toNodeContainer.setInputBuffer(toChannel, i, getGraphData_(graph_).dummyInput);
    }
}

std::uint32_t Buffer::bufferSize() const
{
    return getGraphData_(graph_).bufferSize;
}

void Buffer::setBufferSize(std::uint32_t bufferSize)
{
    auto& pool = getGraphData_(graph_).pool;
    if((!pool) || bufferSize * sizeof(float) > pool->singleBufferByteSize())
    {
        auto newPool = AudioBufferPool::createPool<float>(bufferSize);
        auto newDummyInput = std::make_shared<AudioBufferPool::Buffer>(newPool->lend());
        std::memset(newDummyInput->pointer(), 0, bufferSize * sizeof(float));
        for(const auto& nodeHandle: graph_.nodes())
        {
            auto& container = getNodeData_(graph_, nodeHandle).container;
            const auto& processData = container.audioProcessData();
            std::vector<bool> isDummyInput(processData.inputGroupCount, true);
            std::vector<std::optional<ade::EdgeHandle>> outputs(processData.outputGroupCount, std::nullopt);
            for(auto inEdge: nodeHandle->inEdges())
            {
                isDummyInput[graph_.getEdgeData(inEdge).toChannel] = false;
            }
            for(auto outEdge: nodeHandle->outEdges())
            {
                outputs[graph_.getEdgeData(outEdge).fromChannel] = {outEdge};
            }
            FOR_RANGE0(i, isDummyInput.size())
            {
                if(isDummyInput[i])
                {
                    FOR_RANGE0(j, processData.inputCounts[i])
                    {
                        container.setInputBuffer(i, j, newDummyInput);
                    }
                }
            }
            for(auto& oEdge: outputs)
            {
                if(oEdge.has_value())
                {
                    auto dstNode = (*oEdge)->dstNode();
                    auto& destContainer = getNodeData_(graph_, dstNode).container;
                    auto& [fromChannel, toChannel, data] = graph_.getEdgeData(*oEdge);
                    auto channelCount = processData.outputCounts[fromChannel];
                    FOR_RANGE0(i, channelCount)
                    {
                        auto buffer = std::make_shared<AudioBufferPool::Buffer>(newPool->lend());
                        container.setOutputBuffer(fromChannel, i, buffer);
                        destContainer.setInputBuffer(toChannel, i, buffer);
                    }
                }
            }
        }
        pool = newPool;
        getGraphData_(graph_).dummyInput = newDummyInput;
    }
    for(const auto& nodeHandle: graph_.nodes())
    {
        getNodeData_(graph_, nodeHandle).container.setSingleBufferSize(bufferSize);
        if(auto& callback = getNodeData_(graph_, nodeHandle).bufferSizeChangedCallback)
        {
            callback(bufferSize);
        }
    }
    getGraphData_(graph_).bufferSize = bufferSize;
}

void Buffer::resetBufferSizeChangedCallback(const ade::NodeHandle& nodeHandle)
{
    getNodeData(nodeHandle).bufferSizeChangedCallback = nullptr;
}

const Buffer::NodeData& Buffer::getNodeData(const ade::NodeHandle& nodeHandle) const
{
    return getNodeData_(graph_, nodeHandle);
}

Buffer::NodeData& Buffer::getNodeData(const ade::NodeHandle& nodeHandle)
{
    return getNodeData_(graph_, nodeHandle);
}
}