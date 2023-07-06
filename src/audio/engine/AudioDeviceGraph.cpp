#include "AudioDeviceGraph.hpp"

#include "util/ADEUtil.hpp"

#include <deque>

using YADAW::Audio::Util::SampleDelay;

namespace YADAW::Audio::Engine
{

AudioDeviceGraph::AudioDeviceGraph():
    graph_(),
    typedGraph_(graph_) {}

const AudioDeviceGraph::AudioDeviceProcessNode&
    AudioDeviceGraph::getMetadataFromNode(const ade::NodeHandle& nodeHandle) const
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

AudioDeviceGraph::AudioDeviceProcessNode&
    AudioDeviceGraph::getMetadataFromNode(ade::NodeHandle& nodeHandle)
{
    return typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>();
}

void AudioDeviceGraph::setMetadataFromNode(
    ade::NodeHandle& nodeHandle, AudioDeviceGraph::AudioDeviceProcessNode&& metadata)
{
    typedGraph_.metadata(nodeHandle).set<AudioDeviceProcessNode>(std::move(metadata));
}

ade::NodeHandle AudioDeviceGraph::doAddNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto nodeHandle = graph_.createNode();
    setMetadataFromNode(nodeHandle, AudioDeviceProcessNode{std::move(process), std::move(audioProcessData)});
    return nodeHandle;
}

void AudioDeviceGraph::doRemoveNode(ade::NodeHandle nodeHandle)
{
    typedGraph_.metadata(nodeHandle).erase<AudioDeviceProcessNode>();
    typedGraph_.erase(nodeHandle);
}

ade::EdgeHandle AudioDeviceGraph::doConnect(ade::NodeHandle from, ade::NodeHandle to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto edgeHandle = typedGraph_.link(from, to);
    return edgeHandle;
}

void AudioDeviceGraph::doDisconnect(ade::EdgeHandle edgeHandle)
{
    typedGraph_.metadata(edgeHandle->dstNode()).get<AudioDeviceProcessNode>().upstreamLatency = 0U;
    typedGraph_.erase(edgeHandle);
}


ade::NodeHandle AudioDeviceGraph::addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData)
{
    auto device = process.device();
    auto nodeHandle = doAddNode(std::move(process), std::move(audioProcessData));
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
        for(decltype(audioInputGroupCount) i = 0; i < audioInputGroupCount; ++i)
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
            pdcNode = doAddNode(
                AudioDeviceProcess(pdc),
                std::move(processData)
            );
            doConnect(pdcNode, nodeHandle, 0, i);
        }
    }
    return nodeHandle;
}

void AudioDeviceGraph::removeNode(ade::NodeHandle nodeHandle)
{
    if(auto device = typedGraph_.metadata(nodeHandle).get<AudioDeviceProcessNode>().process.device();
        device->audioInputGroupCount() > 1)
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
        doRemoveNode(nodeHandle);
        pdcs.clear();
        multiInputs_.erase(it);
    }
    else
    {
        doRemoveNode(nodeHandle);
    }
}

ade::EdgeHandle AudioDeviceGraph::connect(ade::NodeHandle from, ade::NodeHandle to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    ade::EdgeHandle ret;
    if(auto device = getMetadataFromNode(to).process.device();
        device->audioInputGroupCount() > 1)
    {
        auto it = multiInputs_.find(to);
        assert(it != multiInputs_.end());
        ret = doConnect(from, it->second[toChannel].first, fromChannel, 0);
        if(getMetadataFromNode(from).sumLatency() > 0)
        {
            onSumLatencyChanged(it->second[toChannel].first);
        }
    }
    else
    {
        auto latencyReduced = getMetadataFromNode(from).sumLatency() > 0;
        ret = doConnect(from, to, fromChannel, toChannel);
        if(latencyReduced)
        {
            onSumLatencyChanged(to);
        }
    }
    return ret;
}

void AudioDeviceGraph::disconnect(ade::EdgeHandle edgeHandle)
{
    auto destNode = edgeHandle->dstNode();
    doDisconnect(edgeHandle);
    onSumLatencyChanged(destNode);
}

void AudioDeviceGraph::disconnect(const std::vector<ade::EdgeHandle>& edgeHandles)
{
    std::vector<ade::NodeHandle> destNodes;
    for(const auto& edgeHandle: edgeHandles)
    {
        auto destNode = edgeHandle->dstNode();
        doDisconnect(edgeHandle);
        if(std::find(destNodes.begin(), destNodes.end(), destNode) == destNodes.end())
        {
            destNodes.emplace_back(destNode);
        }
    }
    for(auto& destNode: destNodes)
    {
        onSumLatencyChanged(destNode);
    }
}

std::optional<YADAW::Util::TopologicalOrderResult<AudioDeviceGraph::AudioDeviceProcessNode>>
    AudioDeviceGraph::topologicalOrder() const
{
    return YADAW::Util::topologicalOrder(typedGraph_);
}

void AudioDeviceGraph::onSumLatencyChanged(ade::NodeHandle nodeHandle)
{
    std::deque<ade::NodeHandle> deque;
    deque.emplace_back(nodeHandle);
    while(!deque.empty())
    {
        auto size = deque.size();
        for(decltype(size) i = 0; i < size; ++i)
        {
            auto& front = deque.front();
            auto& processNode = getMetadataFromNode(front);
            if(processNode.process.device()->audioInputGroupCount() > 1)
            {
                auto inNodes = front->inNodes();
                auto maxUpstreamLatencyNodeIterator = std::max_element(inNodes.begin(), inNodes.end(),
                    [this](const ade::NodeHandle& lhs, const ade::NodeHandle& rhs)
                    {
                        return getMetadataFromNode(lhs).upstreamLatency < getMetadataFromNode(rhs).upstreamLatency;
                    }
                );
                assert(maxUpstreamLatencyNodeIterator != inNodes.end());
                auto maxUpstreamLatency = getMetadataFromNode(*maxUpstreamLatencyNodeIterator).upstreamLatency;
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
                    processNode.upstreamLatency = getMetadataFromNode(inNodes.front()).sumLatency();
                }
            }
            for(auto&& outNode: front->outNodes())
            {
                deque.emplace_back(std::move(outNode));
            }
            deque.pop_front();
        }
    }
    compensate();
}

void AudioDeviceGraph::compensate()
{
    for(auto& [nodeHandle, pdcs]: multiInputs_)
    {
        auto upstreamLatency = getMetadataFromNode(nodeHandle).upstreamLatency;
        for(auto& [pdcNode, pdc]: pdcs)
        {
            if(!(pdcNode->inNodes().empty()))
            {
                auto processing = pdc.isProcessing();
                if(processing)
                {
                    pdc.stopProcessing();
                }
                pdc.setDelay(upstreamLatency - getMetadataFromNode(pdcNode).upstreamLatency);
                if(processing)
                {
                    pdc.startProcessing();
                }
            }
        }
    }
}
}