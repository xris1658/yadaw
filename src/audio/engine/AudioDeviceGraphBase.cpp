#include "AudioDeviceGraphBase.hpp"

#include "util/ADEUtil.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Util::AudioBufferPool;

void blankAfterAddNodeCallback(
    AudioDeviceGraphBase&, const ade::NodeHandle&)
{}

void blankBeforeRemoveNodeCallback(
    AudioDeviceGraphBase&, const ade::NodeHandle&)
{}

void blankAfterConnectCallback(
    AudioDeviceGraphBase&, const ade::EdgeHandle&)
{}

void blankBeforeDisconnectCallback(
    AudioDeviceGraphBase&, const ade::EdgeHandle&)
{}

AudioDeviceGraphBase::AudioDeviceGraphBase():
    graph_(),
    typedGraph_(graph_),
    afterConnectCallback_(blankAfterConnectCallback),
    beforeDisconnectCallback_(blankBeforeDisconnectCallback)
{}

AudioDeviceGraphBase::~AudioDeviceGraphBase()
{
    clear();
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

ade::NodeHandle AudioDeviceGraphBase::addNode(AudioDeviceProcess&& process)
{
    auto ret = typedGraph_.createNode();
    auto& device = *(process.device());
    typedGraph_.metadata(ret).set<NodeData>(
        NodeData{std::move(process), nullptr});
    afterAddNodeCallback_(*this, ret);
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
    beforeRemoveNodeCallback_(*this, nodeHandle);
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
                    return edgeHandle->dstNode() != toNode || edgeData.toChannel != toChannel;
                }
            )
        )
        {
            auto& [fromDeviceProcess, fromData] = getNodeData(fromNode);
            auto& [toDeviceProcess, toData] = getNodeData(toNode);
            auto fromDevice = fromDeviceProcess.device();
            auto toDevice = toDeviceProcess.device();
            if(fromChannel < fromDevice->audioOutputGroupCount()
               && toChannel < toDevice->audioInputGroupCount()
               && fromDevice->audioOutputGroupAt(fromChannel)->get().channelCount()
                  == toDevice->audioInputGroupAt(toChannel)->get().channelCount())
            {
                auto ret = typedGraph_.link(fromNode, toNode);
                typedGraph_.metadata(ret).set<EdgeData>({fromChannel, toChannel, nullptr});
                afterConnectCallback_(*this, ret);
                return {ret};
            }
        }
    }
    return std::nullopt;
}

void AudioDeviceGraphBase::disconnect(const ade::EdgeHandle& edgeHandle)
{
    auto& [toDeviceProcess, toNodeData] = getNodeData(edgeHandle->dstNode());
    auto& [fromChannel, toChannel, edgeData] = getEdgeData(edgeHandle);
    beforeDisconnectCallback_(*this, edgeHandle);
    typedGraph_.erase(edgeHandle);
}

void AudioDeviceGraphBase::clear()
{
    FOR_RANGE0(i, graph_.nodes().size())
    {
        removeNode(graph_.nodes().front());
    }
}

void AudioDeviceGraphBase::setAfterAddNodeCallback(std::function<AfterAddNodeCallback>&& func)
{
    afterAddNodeCallback_ = std::move(func);
}

void AudioDeviceGraphBase::setBeforeRemoveNodeCallback(std::function<BeforeRemoveNodeCallback>&& func)
{
    beforeRemoveNodeCallback_ = std::move(func);
}

void AudioDeviceGraphBase::setAfterConnectCallback(std::function<AfterConnectCallback>&& func)
{
    afterConnectCallback_ = std::move(func);
}

void AudioDeviceGraphBase::setBeforeDisconnectCallback(std::function<BeforeDisconnectCallback>&& func)
{
    beforeDisconnectCallback_ = std::move(func);
}

void AudioDeviceGraphBase::resetAfterAddNodeCallback()
{
    setAfterAddNodeCallback({blankAfterAddNodeCallback});
}

void AudioDeviceGraphBase::resetBeforeRemoveNodeCallback()
{
    setBeforeRemoveNodeCallback({blankBeforeRemoveNodeCallback});
}

void AudioDeviceGraphBase::resetAfterConnectCallback()
{
    setAfterConnectCallback({blankAfterConnectCallback});
}

void AudioDeviceGraphBase::resetBeforeDisconnectCallback()
{
    setBeforeDisconnectCallback({blankBeforeDisconnectCallback});
}

std::vector<std::vector<std::vector<ade::NodeHandle>>> AudioDeviceGraphBase::topologicalSort() const
{
    std::vector<std::vector<std::vector<ade::NodeHandle>>> ret;
    auto topo = YADAW::Util::topologicalSort(YADAW::Util::squashGraph(graph_));
    if(topo.has_value())
    {
        auto& topoSortResult = *topo;
        ret.reserve(topoSortResult.size());
        for(auto& row: topoSortResult)
        {
            auto& retRow = ret.emplace_back();
            retRow.reserve(row.size());
            for(auto& [from, to]: row)
            {
                auto& retCell = retRow.emplace_back();
                auto i = from;
                while(true)
                {
                    retCell.emplace_back(i);
                    if(i == to)
                    {
                        break;
                    }
                    i = i->outNodes().front();
                }
            }
        }
    }
    return ret;
}
}