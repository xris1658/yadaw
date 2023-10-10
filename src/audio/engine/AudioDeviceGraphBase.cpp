#include "AudioDeviceGraphBase.hpp"

#include "util/ADEUtil.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Util::AudioBufferPool;

AudioDeviceGraphBase::AudioDeviceGraphBase(std::uint32_t bufferSize):
    graph_(),
    typedGraph_(graph_),
    bufferSize_(bufferSize),
    pool_(AudioBufferPool::createPool<float>(bufferSize)),
    dummyInput_(std::make_shared<AudioBufferPool::Buffer>(pool_->lend()))
{
    auto& rDummyInput = *dummyInput_;
    std::memset(rDummyInput.pointer(), 0, bufferSize_ * sizeof(float));
}

const AudioDeviceGraphBase::NodeData& AudioDeviceGraphBase::getNodeData(
    const ade::NodeHandle& nodeHandle) const
{
    return typedGraph_.metadata(nodeHandle).get<NodeData>();
}

AudioDeviceGraphBase::NodeData& AudioDeviceGraphBase::getNodeData(
    const ade::NodeHandle& nodeHandle)
{
    return typedGraph_.metadata(nodeHandle).get<NodeData>();
}

const AudioDeviceGraphBase::EdgeData& AudioDeviceGraphBase::getEdgeData(
    const ade::EdgeHandle& edgeHandle) const
{
    return typedGraph_.metadata(edgeHandle).get<EdgeData>();
}

AudioDeviceGraphBase::EdgeData& AudioDeviceGraphBase::getEdgeData(
    const ade::EdgeHandle& edgeHandle)
{
    return typedGraph_.metadata(edgeHandle).get<EdgeData>();
}

std::uint32_t AudioDeviceGraphBase::bufferSize() const
{
    return bufferSize_;
}

void AudioDeviceGraphBase::setBufferSize(std::uint32_t bufferSize)
{
    if(bufferSize * sizeof(float) > pool_->singleBufferByteSize())
    {
        auto pool = AudioBufferPool::createPool<float>(bufferSize);
        auto dummyInput = std::make_shared<AudioBufferPool::Buffer>(pool->lend());
        for(auto nodeHandle: typedGraph_.nodes())
        {
            auto& nodeData = getNodeData(nodeHandle);
            auto device = nodeData.process.device();
            auto& container = nodeData.processData;
            const auto& processData = container.audioProcessData();
            std::vector<bool> isDummyInput(processData.inputGroupCount, true);
            std::vector<std::optional<ade::EdgeHandle>> outputs(processData.outputGroupCount, std::nullopt);
            for(auto inEdge: nodeHandle->inEdges())
            {
                isDummyInput[getEdgeData(inEdge).toChannel] = false;
            }
            for(auto outEdge: nodeHandle->outEdges())
            {
                outputs[getEdgeData(outEdge).fromChannel] = {outEdge};
            }
            FOR_RANGE0(i, isDummyInput.size())
            {
                if(isDummyInput[i])
                {
                    FOR_RANGE0(j, processData.inputCounts[i])
                    {
                        container.setInputBuffer(i, j, dummyInput);
                    }
                }
            }
            for(auto& oEdge: outputs)
            {
                if(oEdge.has_value())
                {
                    auto dstNode = (*oEdge)->dstNode();
                    auto& destContainer = getNodeData(dstNode).processData;
                    auto& [fromChannel, toChannel, data] = getEdgeData(*oEdge);
                    auto channelCount = processData.outputCounts[fromChannel];
                    FOR_RANGE0(i, channelCount)
                    {
                        auto buffer = std::make_shared<AudioBufferPool::Buffer>(pool->lend());
                        container.setOutputBuffer(fromChannel, i, buffer);
                        destContainer.setInputBuffer(toChannel, i, buffer);
                    }
                }
            }
        }
        pool_ = std::move(pool);
        dummyInput_ = std::move(dummyInput);
    }
    for(auto nodeHandle: typedGraph_.nodes())
    {
        getNodeData(nodeHandle).processData.setSingleBufferSize(bufferSize);
    }
    bufferSize_ = bufferSize;
}

ade::NodeHandle AudioDeviceGraphBase::addNode(AudioDeviceProcess&& process)
{
    auto ret = typedGraph_.createNode();
    auto& device = *(process.device());
    YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> container;
    container.setSingleBufferSize(bufferSize_);
    const auto inputGroupCount = device.audioInputGroupCount();
    container.setInputGroupCount(inputGroupCount);
    FOR_RANGE0(i, inputGroupCount)
    {
        const auto inputCount = device.audioInputGroupAt(i)->get().channelCount();
        container.setInputCount(i, inputCount);
        FOR_RANGE0(j, inputCount)
        {
            container.setInputBuffer(i, j, dummyInput_);
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
                i, j, std::make_shared<AudioBufferPool::Buffer>(pool_->lend())
            );
        }
    }
    return ret;
}

void AudioDeviceGraphBase::removeNode(const ade::NodeHandle& nodeHandle)
{
    auto edgeCount = nodeHandle->inEdges().size();
    FOR_RANGE0(i, edgeCount)
    {
        disconnect(nodeHandle->inEdges().front());
    }
    edgeCount = nodeHandle->outEdges().size();
    FOR_RANGE0(i, edgeCount)
    {
        disconnect(nodeHandle->outEdges().front());
    }
    typedGraph_.erase(nodeHandle);
}

std::optional<ade::EdgeHandle> AudioDeviceGraphBase::connect(
    const ade::NodeHandle& fromNode, const ade::NodeHandle& toNode,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    if(!YADAW::Util::pathExists(toNode, fromNode))
    {
        if(auto outEdges = fromNode->outEdges();
            std::all_of(
                outEdges.begin(), outEdges.end(),
                [this, &toNode, fromChannel, toChannel]
                    (const ade::EdgeHandle& edgeHandle)
                {
                    const auto& edgeData = getEdgeData(edgeHandle);
                    return edgeData.fromChannel != fromChannel && (
                        edgeHandle->dstNode() != toNode ||
                        edgeData.toChannel != toChannel
                    );
                }
            )
        )
        {
            auto& [fromDeviceProcess, fromProcessData, fromData] = getNodeData(fromNode);
            auto& [toDeviceProcess, toProcessData, toData] = getNodeData(toNode);
            auto fromDevice = fromDeviceProcess.device();
            auto toDevice = toDeviceProcess.device();
            if(fromChannel < fromDevice->audioOutputGroupCount()
               && toChannel < toDevice->audioInputGroupCount()
               && fromDevice->audioOutputGroupAt(fromChannel)->get().channelCount()
                  == toDevice->audioInputGroupAt(toChannel)->get().channelCount())
            {
                FOR_RANGE0(i, toProcessData.audioProcessData().inputCounts[toChannel])
                {
                    toProcessData.setInputBuffer(
                        toChannel, i, fromProcessData.outputBuffer(fromChannel, i));
                }
                auto ret = typedGraph_.link(fromNode, toNode);
                typedGraph_.metadata(ret).set<EdgeData>({fromChannel, toChannel});
                return {ret};
            }
        }
    }
    return std::nullopt;
}

void AudioDeviceGraphBase::disconnect(const ade::EdgeHandle& edgeHandle)
{
    auto& [toDeviceProcess, toProcessData, toNodeData] = getNodeData(edgeHandle->dstNode());
    auto& [fromChannel, toChannel, edgeData] = getEdgeData(edgeHandle);
    FOR_RANGE0(i, toProcessData.audioProcessData().inputCounts[toChannel])
    {
        toProcessData.setInputBuffer(toChannel, i, dummyInput_);
    }
    typedGraph_.erase(edgeHandle);
}

AudioDeviceGraphBase::TopologicalSortResult AudioDeviceGraphBase::topologicalSort() const
{
    auto topoResult = YADAW::Util::topologicalSort(
        YADAW::Util::squashGraph(graph_)
    );
    auto& result = *topoResult;
    TopologicalSortResult ret;
    ret.reserve(result.size());
    for(const auto& row: result)
    {
        auto& rowInRet = ret.emplace_back();
        rowInRet.reserve(row.size());
        for(const auto& cell: row)
        {
            auto& cellInRet = rowInRet.emplace_back();
            auto i = cell.first;
            while(true)
            {
                cellInRet.emplace_back(getNodeData(i));
                if(i == cell.second)
                {
                    break;
                }
                i = i->outNodes().front();
            }
        }
    }
    return ret;
}
}