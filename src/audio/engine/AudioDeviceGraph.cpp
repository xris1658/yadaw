#include "AudioDeviceGraph.hpp"

#include "util/ADEUtil.hpp"
#include "util/IntegerRange.hpp"

#include <deque>

using YADAW::Audio::Util::SampleDelay;

namespace YADAW::Audio::Engine
{

AudioDeviceGraph::AudioDeviceGraph(bool latencyCompensationEnabled):
    graph_(),
    typedGraph_(graph_),
    latencyCompensationEnabled_(latencyCompensationEnabled)
{}

const AudioDeviceGraph::AudioDeviceProcessNode&
    AudioDeviceGraph::getMetadata(const ade::NodeHandle& nodeHandle) const
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

const AudioDeviceGraph::EdgeData& AudioDeviceGraph::getMetadata(const ade::EdgeHandle& edgeHandle)
{
    return typedGraph_.metadata(edgeHandle).get<EdgeData>();
}

AudioDeviceGraph::AudioDeviceProcessNode&
    AudioDeviceGraph::getMetadata(ade::NodeHandle& nodeHandle)
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

void AudioDeviceGraph::setMetadata(
    ade::NodeHandle& nodeHandle, AudioDeviceProcessNode&& metadata)
{
    typedGraph_.metadata(nodeHandle).set<AudioDeviceProcessNode>(std::move(metadata));
}

ade::NodeHandle AudioDeviceGraph::doAddNode(
    AudioDeviceProcess&& process, const AudioProcessData<float>& audioProcessData)
{
    auto nodeHandle = graph_.createNode();
    setMetadata(nodeHandle, AudioDeviceProcessNode {std::move(process), audioProcessData});
    return nodeHandle;
}

ade::NodeHandle AudioDeviceGraph::doAddNode(
    AudioDeviceProcess&& process, AudioProcessData<float>& audioProcessData)
{
    auto nodeHandle = graph_.createNode();
    setMetadata(nodeHandle, AudioDeviceProcessNode {std::move(process), audioProcessData});
    return nodeHandle;
}

void AudioDeviceGraph::doRemoveNode(ade::NodeHandle nodeHandle)
{
    typedGraph_.metadata(nodeHandle).erase<AudioDeviceProcessNode>();
    typedGraph_.erase(nodeHandle);
}

ade::EdgeHandle AudioDeviceGraph::doConnect(
    ade::NodeHandle from, ade::NodeHandle to, EdgeData edgeData)
{
    auto ret = typedGraph_.link(from, to);
    typedGraph_.metadata(ret).set<EdgeData>(std::move(edgeData));
    return ret;
}

void AudioDeviceGraph::doDisconnect(ade::EdgeHandle edgeHandle)
{
    typedGraph_.metadata(edgeHandle->dstNode()).get<AudioDeviceProcessNode>().upstreamLatency = 0U;
    typedGraph_.erase(edgeHandle);
}

void AudioDeviceGraph::doAddLatencyCompensation(const ade::NodeHandle& nodeHandle)
{
    auto& metadata = getMetadata(nodeHandle);
    auto device = metadata.process.device();
    const auto& audioProcessData = metadata.processData;
    if(auto audioInputGroupCount = device->audioInputGroupCount();
        audioInputGroupCount > 1)
    {
        auto [it, inserted] = multiInputs_.emplace(
            std::make_pair(
                nodeHandle,
                decltype(multiInputs_)::value_type::second_type()
            )
        );
        auto& pdcs = it->second;
        pdcs.reserve(audioInputGroupCount);
        FOR_RANGE0(i, audioInputGroupCount)
        {
            auto& [pdcNode, pdc] = pdcs.emplace_back(ade::NodeHandle(), SampleDelay(0, device->audioInputGroupAt(i)->get()));
            AudioProcessData<float> processData;
            processData.singleBufferSize = audioProcessData.singleBufferSize;
            processData.inputGroupCount = 1;
            processData.outputGroupCount = 1;
            processData.outputCounts = audioProcessData.inputCounts + i;
            processData.outputs = audioProcessData.inputs + i;
            processData.inputCounts = processData.outputCounts;
            processData.inputs = processData.outputs;
            pdc.startProcessing();
            pdcNode = doAddNode(AudioDeviceProcess(pdc), std::move(processData));
            doConnect(pdcNode, nodeHandle, {0U, i});
        }
    }
    if(auto audioOutputGroupCount = device->audioOutputGroupCount();
        audioOutputGroupCount > 1)
    {
        multiOutputs_.emplace(nodeHandle);
    }
}

ade::NodeHandle AudioDeviceGraph::addNode(AudioDeviceProcess&& process, const AudioProcessData<float>& audioProcessData)
{
    auto nodeHandle = doAddNode(std::move(process), audioProcessData);
    doAddLatencyCompensation(nodeHandle);
    return nodeHandle;
}

ade::NodeHandle AudioDeviceGraph::addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto nodeHandle = doAddNode(std::move(process), std::move(audioProcessData));
    doAddLatencyCompensation(nodeHandle);
    return nodeHandle;
}

void AudioDeviceGraph::removeNode(ade::NodeHandle nodeHandle)
{
    auto device = typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>().process.device();
    if(device->audioInputGroupCount() > 1)
    {
        auto it = multiInputs_.find(nodeHandle);
        assert(it != multiInputs_.end());
        auto& pdcs = it->second;
        for(auto& [pdcNodeHandle, pdc]: pdcs)
        {
            pdc.stopProcessing();
            doDisconnect(pdcNodeHandle->outEdges().front());
            doRemoveNode(pdcNodeHandle);
        }
        pdcs.clear();
        multiInputs_.erase(it);
    }
    if(device->audioOutputGroupCount() > 1)
    {
        auto it = multiOutputs_.find(nodeHandle);
        assert(it != multiOutputs_.end());
        multiOutputs_.erase(it);
    }
    doRemoveNode(nodeHandle);
}

ade::EdgeHandle AudioDeviceGraph::connect(ade::NodeHandle from, ade::NodeHandle to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    ade::EdgeHandle ret;
    if(!YADAW::Util::pathExists(to, from))
    {
        auto fromDevice = getMetadata(from).process.device();
        auto toDevice = getMetadata(to).process.device();
        if(fromChannel < fromDevice->audioOutputGroupCount()
            && toChannel < toDevice->audioInputGroupCount()
            && fromDevice->audioOutputGroupAt(fromChannel)->get().channelCount()
                == toDevice->audioInputGroupAt(toChannel)->get().channelCount()
        )
        {
            if(toDevice->audioInputGroupCount() > 1)
            {
                auto it = multiInputs_.find(to);
                assert(it != multiInputs_.end());
                ret = doConnect(from, it->second[toChannel].first, {to, fromChannel, toChannel});
                if(getMetadata(from).sumLatency() > 0)
                {
                    onSumLatencyChanged(it->second[toChannel].first);
                }
            }
            else
            {
                auto latencyReduced = getMetadata(from).sumLatency() > 0;
                ret = doConnect(from, to, {to, fromChannel, toChannel});
                if(latencyReduced)
                {
                    onSumLatencyChanged(to);
                }
            }
        }
    }
    return ret;
}

void AudioDeviceGraph::disconnect(ade::EdgeHandle edgeHandle)
{
    if(edgeHandle.get())
    {
        auto destNode = edgeHandle->dstNode();
        doDisconnect(edgeHandle);
        onSumLatencyChanged(destNode);
    }
}

void AudioDeviceGraph::disconnect(const std::vector<ade::EdgeHandle>& edgeHandles)
{
    std::vector<ade::NodeHandle> destNodes;
    for(const auto& edgeHandle: edgeHandles)
    {
        if(edgeHandle.get())
        {
            auto destNode = edgeHandle->dstNode();
            doDisconnect(edgeHandle);
            if(std::find(destNodes.begin(), destNodes.end(), destNode) == destNodes.end())
            {
                destNodes.emplace_back(destNode);
            }
        }
    }
    for(auto& destNode: destNodes)
    {
        onSumLatencyChanged(destNode);
    }
}

AudioDeviceGraph::TopologicalSortResult AudioDeviceGraph::topologicalSort() const
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
                cellInRet.emplace_back(getMetadata(i));
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

void AudioDeviceGraph::onSumLatencyChanged(ade::NodeHandle nodeHandle)
{
    std::deque<ade::NodeHandle> deque;
    deque.emplace_back(nodeHandle);
    while(!deque.empty())
    {
        auto size = deque.size();
        FOR_RANGE0(i, size)
        {
            auto& front = deque.front();
            auto& processNode = getMetadata(front);
            if(processNode.process.device()->audioInputGroupCount() > 1)
            {
                auto inNodes = front->inNodes();
                auto maxUpstreamLatencyNodeIterator = std::max_element(inNodes.begin(), inNodes.end(),
                    [this](const ade::NodeHandle& lhs, const ade::NodeHandle& rhs)
                    {
                        return getMetadata(lhs).upstreamLatency < getMetadata(rhs).upstreamLatency;
                    }
                );
                assert(maxUpstreamLatencyNodeIterator != inNodes.end());
                auto maxUpstreamLatency = getMetadata(*maxUpstreamLatencyNodeIterator).upstreamLatency;
                processNode.upstreamLatency = maxUpstreamLatency;
            }
            else
            {
                if(const auto& inNodes = front->inNodes(); inNodes.empty())
                {
                    processNode.upstreamLatency = 0;
                }
                else
                {
                    processNode.upstreamLatency = getMetadata(inNodes.front()).sumLatency();
                }
            }
            for(auto&& outNode: front->outNodes())
            {
                deque.emplace_back(std::move(outNode));
            }
            deque.pop_front();
        }
    }
    compensate(latencyCompensationEnabled_);
}

void AudioDeviceGraph::compensate(bool latencyCompensationEnabled)
{
    if(latencyCompensationEnabled)
    {
        for(auto& [nodeHandle, pdcs]: multiInputs_)
        {
            auto upstreamLatency = getMetadata(nodeHandle).upstreamLatency;
            for(auto& [pdcNode, pdc]: pdcs)
            {
                if(!(pdcNode->inNodes().empty()))
                {
                    auto processing = pdc.isProcessing();
                    if(processing)
                    {
                        pdc.stopProcessing();
                    }
                    pdc.setDelay(upstreamLatency - getMetadata(pdcNode).upstreamLatency);
                    if(processing)
                    {
                        pdc.startProcessing();
                    }
                }
            }
        }
    }
    else
    {
        for(auto& [nodeHandle, pdcs]: multiInputs_)
        {
            for(auto& [pdcNode, pdc]: pdcs)
            {
                auto processing = pdc.isProcessing();
                if(processing)
                {
                    pdc.stopProcessing();
                }
                pdc.setDelay(0);
                if(processing)
                {
                    pdc.startProcessing();
                }
            }
        }
    }
}

bool AudioDeviceGraph::latencyCompensationEnabled() const
{
    return latencyCompensationEnabled_;
}

void AudioDeviceGraph::setLatencyCompensationEnabled(bool enabled)
{
    if(latencyCompensationEnabled_ != enabled)
    {
        compensate(enabled);
        latencyCompensationEnabled_ = enabled;
    }
}
}