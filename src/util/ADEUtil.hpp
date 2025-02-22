#ifndef YADAW_SRC_UTIL_ADEUTIL
#define YADAW_SRC_UTIL_ADEUTIL

#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <algorithm>
#include <optional>
#include <set>
#include <vector>

namespace YADAW::Util
{
namespace Impl
{
template<bool In, typename Predicate>
std::optional<ade::EdgeHandle> findFirstPassedEdge(
    const ade::Graph& graph, const ade::NodeHandle& nodeHandle, Predicate predicate)
{
    static_assert(std::is_invocable_r_v<bool, Predicate, const ade::Graph&, const ade::EdgeHandle&>);
    auto edges = In? nodeHandle->inEdges(): nodeHandle->outEdges();
    auto it = std::find_if(edges.begin(), edges.end(),
        [&graph, &predicate](const ade::EdgeHandle& edge)
        {
            return predicate(graph, edge);
        }
    );
    return it == edges.end()? std::nullopt: std::optional(*it);
}
}

// Describes a link in a `ade::Graph`.
// Link is inclusive, meaning that both `first` and `second` are part of it.
using Link = std::pair<ade::NodeHandle, ade::NodeHandle>;

template<typename T>
using TopologicalOrderResult = std::vector<std::vector<T>>;

bool pathExists(const ade::NodeHandle& from, const ade::NodeHandle& to);

// Squash a `ade::Graph` that is a DAG. Cycles are not included.
std::vector<Link> squashGraph(const ade::Graph& graph);

template<typename Predicate>
std::vector<Link> squashGraphWithEdgeCheck(const ade::Graph& graph, Predicate predicate)
{
    static_assert(std::is_invocable_r_v<bool, Predicate, const ade::Graph&, const ade::EdgeHandle&>);
    std::unordered_set<ade::NodeHandle, ade::HandleHasher<ade::Node>> linkStartNodes;
    std::unordered_set<ade::NodeHandle, ade::HandleHasher<ade::Node>> linkEndNodes;
    for(const auto& nodeHandle: graph.nodes())
    {
        std::optional<ade::NodeHandle> firstInNode;
        std::optional<ade::NodeHandle> firstOutNode;
        for(const auto& inEdge: nodeHandle->inEdges())
        {
            if(predicate(graph, inEdge))
            {
                if(!firstInNode.has_value())
                {
                    firstInNode.emplace(inEdge->srcNode());
                }
                else if(*firstInNode != inEdge->srcNode())
                {
                    firstInNode.reset();
                    break;
                }
            }
        }
        for(const auto& outEdge: nodeHandle->outEdges())
        {
            if(predicate(graph, outEdge))
            {
                if(!firstOutNode.has_value())
                {
                    firstOutNode.emplace(outEdge->dstNode());
                }
                else if(*firstOutNode != outEdge->dstNode())
                {
                    firstOutNode.reset();
                    break;
                }
            }
        }
        if(!firstInNode.has_value())
        {
            linkStartNodes.emplace(nodeHandle);
            for(const auto& inEdge: nodeHandle->inEdges())
            {
                linkEndNodes.insert(inEdge->srcNode());
            }
        }
        if(!firstOutNode.has_value())
        {
            linkEndNodes.emplace(nodeHandle);
            for(const auto& outEdge: nodeHandle->outEdges())
            {
                linkStartNodes.insert(outEdge->dstNode());
            }
        }
    }
    std::vector<Link> ret;
    for(const auto& nodeHandle: linkStartNodes)
    {
        auto i = nodeHandle;
        while(linkEndNodes.find(i) == linkEndNodes.end())
        {
            auto outEdge = Impl::findFirstPassedEdge<false>(graph, i, predicate);
            if(!outEdge.has_value())
            {
                break;
            }
            i = (*outEdge)->dstNode();
        }
        ret.emplace_back(nodeHandle, i);
    }
    for(const auto& nodeHandle: linkEndNodes)
    {
        auto i = nodeHandle;
        while(linkStartNodes.find(i) == linkStartNodes.end())
        {
            auto inEdge = Impl::findFirstPassedEdge<true>(graph, i, predicate);
            if(!inEdge.has_value())
            {
                break;
            }
            i = (*inEdge)->srcNode();
        }
        auto it = linkStartNodes.find(i);
        if(it == linkStartNodes.end())
        {
            ret.emplace_back(i, nodeHandle);
        }
    }
    return ret;
}

// Do topological sort on a squashed `ade::Graph`.
std::optional<std::vector<std::vector<Link>>> topologicalSort(
    const ade::Graph& graph,
    const std::vector<Link>& squashedGraph
);

template<typename Predicate>
std::optional<std::vector<std::vector<Link>>> topologicalSort(
    const ade::Graph& graph,
    const std::vector<Link>& squashedGraph,
    Predicate predicate
)
{
    static_assert(std::is_invocable_r_v<bool, Predicate, const ade::Graph&, const ade::EdgeHandle&>);
    std::vector<std::vector<Link>> ret;
    if(!squashedGraph.empty())
    {
        std::vector<Link> visitedLinks;
        std::vector<Link> unvisitedLinks = squashedGraph;
        do
        {
            auto partitionPoint = std::partition(
                unvisitedLinks.begin(), unvisitedLinks.end(),
                [&graph, &visitedLinks, &predicate](const Link& link)
                {
                    auto inEdges = link.first->inEdges();
                    // `std::any_of` returns `false` if the input range is
                    // empty, which means that the node has no in edges.
                    return std::any_of(inEdges.begin(), inEdges.end(),
                        [&graph, &visitedLinks, &predicate](const ade::EdgeHandle& inEdge)
                        {
                            return predicate(graph, inEdge) && std::find_if(
                                visitedLinks.begin(), visitedLinks.end(),
                                [inNode = inEdge->srcNode()](const Link& visitedLink)
                                {
                                    return visitedLink.second == inNode;
                                }
                            ) == visitedLinks.end();
                        }
                    );
                }
            );
            if(partitionPoint == unvisitedLinks.end())
            {
                return std::nullopt;
            }
            auto& row = ret.emplace_back();
            row.reserve(unvisitedLinks.end() - partitionPoint);
            std::copy(partitionPoint, unvisitedLinks.end(),
                std::back_inserter(row));
            std::move(partitionPoint, unvisitedLinks.end(),
                std::back_inserter(visitedLinks));
            unvisitedLinks.erase(partitionPoint, unvisitedLinks.end());
        }
        while(!unvisitedLinks.empty());
    }
    return {ret};
}

struct CompareNodeHandle
{
    bool operator()(const ade::NodeHandle& lhs, const ade::NodeHandle& rhs) const;
};
}

#endif // YADAW_SRC_UTIL_ADEUTIL
