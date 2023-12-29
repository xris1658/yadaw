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
// Describes a link in a `ade::Graph`.
// Link is inclusive, meaning that both `first` and `second` are part of it.
using Link = std::pair<ade::NodeHandle, ade::NodeHandle>;

template<typename T>
using TopologicalOrderResult = std::vector<std::vector<T>>;

bool pathExists(ade::NodeHandle from, ade::NodeHandle to);

template<typename T>
std::optional<TopologicalOrderResult<T>> topologicalOrder(const ade::TypedGraph<T>& graph)
{
    TopologicalOrderResult<T> ret;
    std::vector<ade::NodeHandle> visitedNodes;
    std::vector<ade::NodeHandle> unvisitedNodes;
    for(const auto& node: graph.nodes())
    {
        unvisitedNodes.emplace_back(node);
    }
    std::vector<ade::NodeHandle> currentPassNodes;
    while(!unvisitedNodes.empty())
    {
        std::vector<T> currentPassValue;
        auto size = visitedNodes.size();
        for(auto it = unvisitedNodes.begin(); it != unvisitedNodes.end(); ++it)
        {
            auto& node = *it;
            auto inNodes = node->inNodes();
            if((inNodes.size() <= size)
                && std::all_of(inNodes.begin(), inNodes.end(),
                    [&visitedNodes](const ade::NodeHandle& nodeHandle)
                    {
                        return std::find(visitedNodes.begin(), visitedNodes.end(), nodeHandle)
                            != visitedNodes.end();
                    }
                )
            )
            {
                currentPassNodes.emplace_back(node);
            }
        }
        if(currentPassNodes.empty())
        {
            return std::nullopt;
        }
        for(const auto& currentPassNode: currentPassNodes)
        {
            visitedNodes.emplace_back(currentPassNode);
            currentPassValue.emplace_back(graph.metadata(currentPassNode).template get<T>());
        }
        ret.emplace_back(std::move(currentPassValue));
        unvisitedNodes.erase(
            std::remove_if(unvisitedNodes.begin(), unvisitedNodes.end(),
                [&currentPassNodes](const ade::NodeHandle& nodeHandle)
                {
                    return std::find(currentPassNodes.begin(), currentPassNodes.end(), nodeHandle) != currentPassNodes.end();
                }
            ),
            unvisitedNodes.end()
        );
        currentPassNodes.clear();
    }
    return {ret};
}

// Squash a `ade::Graph` that is a DAG. Cycles are not included.
std::vector<Link> squashGraph(const ade::Graph& graph);

// Do topological sort on a squashed `ade::Graph`.
std::optional<std::vector<std::vector<Link>>> topologicalSort(
    const std::vector<Link>& squashedGraph
);

struct CompareNodeHandle
{
    bool operator()(const ade::NodeHandle& lhs, const ade::NodeHandle& rhs) const;
};
}

#endif // YADAW_SRC_UTIL_ADEUTIL
