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
    afterAddNodeCallback_(blankAfterAddNodeCallback),
    beforeRemoveNodeCallback_(blankBeforeRemoveNodeCallback),
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
                [this, &toNode, toChannel]
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

void AudioDeviceGraphBase::setAfterAddNodeCallback(AfterAddNodeCallback* func)
{
    afterAddNodeCallback_ = func;
}

void AudioDeviceGraphBase::setBeforeRemoveNodeCallback(BeforeRemoveNodeCallback* func)
{
    beforeRemoveNodeCallback_ = func;
}

void AudioDeviceGraphBase::setAfterConnectCallback(AfterConnectCallback* func)
{
    afterConnectCallback_ = func;
}

void AudioDeviceGraphBase::setBeforeDisconnectCallback(BeforeDisconnectCallback* func)
{
    beforeDisconnectCallback_ = func;
}

void AudioDeviceGraphBase::resetAfterAddNodeCallback()
{
    setAfterAddNodeCallback(&blankAfterAddNodeCallback);
}

void AudioDeviceGraphBase::resetBeforeRemoveNodeCallback()
{
    setBeforeRemoveNodeCallback(&blankBeforeRemoveNodeCallback);
}

void AudioDeviceGraphBase::resetAfterConnectCallback()
{
    setAfterConnectCallback(&blankAfterConnectCallback);
}

void AudioDeviceGraphBase::resetBeforeDisconnectCallback()
{
    setBeforeDisconnectCallback(&blankBeforeDisconnectCallback);
}

std::vector<std::vector<std::vector<ade::NodeHandle>>> AudioDeviceGraphBase::topologicalSort() const
{
    std::vector<std::vector<std::vector<ade::NodeHandle>>> ret;
    auto topo = YADAW::Util::topologicalSort(graph_, YADAW::Util::squashGraph(graph_));
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

Vec<Vec<Vec<ade::NodeHandle>, Vec<std::uint32_t, std::uint32_t>>>
    AudioDeviceGraphBase::topologicalSortWithPrev() const
{
    Vec<Vec<Vec<ade::NodeHandle>, Vec<std::uint32_t, std::uint32_t>>> ret;
    auto topo = YADAW::Util::topologicalSort(graph_, YADAW::Util::squashGraph(graph_));
    if(topo.has_value())
    {
        auto& topoSortResult = *topo;
        ret.reserve(topoSortResult.size());
        FOR_RANGE0(i, topoSortResult.size())
        {
            auto& row = topoSortResult[i];
            auto& retRow = ret.emplace_back();
            retRow.reserve(row.size());
            for(auto& [from, to]: row)
            {
                auto fromInEdges = from->inEdges();
                auto& [retCell, retPrev] = retRow.emplace_back();
                for(auto i = from; ; i = i->outNodes().front())
                {
                    retCell.emplace_back(i);
                    if(i == to)
                    {
                        break;
                    }
                }
                auto inEdgeCount = fromInEdges.size();
                retPrev.reserve(inEdgeCount);
                FOR_RANGE0(k, i)
                {
                    auto& topoRow = topoSortResult[k];
                    FOR_RANGE0(l, topoRow.size())
                    {
                        const auto& outNode = topoRow[l].second;
                        if(
                            std::find_if(
                                fromInEdges.begin(), fromInEdges.end(),
                                [&outNode](const ade::EdgeHandle& edgeHandle)
                                {
                                    return edgeHandle->srcNode() == outNode;
                                }
                            ) != fromInEdges.end())
                        {
                            retPrev.emplace_back(k, l);
                        }
                    }
                }
            }
        }
    }
    return ret;
}
}
