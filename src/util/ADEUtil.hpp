#ifndef YADAW_SRC_UTIL_ADEUTIL
#define YADAW_SRC_UTIL_ADEUTIL

#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <optional>
#include <set>
#include <vector>

namespace YADAW::Util
{
template<typename T>
using TopologicalOrderResult = std::vector<std::vector<T>>;

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
    while(!unvisitedNodes.empty())
    {
        std::vector<ade::NodeHandle> currentPassNodes;
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
            currentPassValue.emplace_back(graph.metadata(currentPassNode).get<T>());
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
    }
    return {ret};
}
}

#endif // YADAW_SRC_UTIL_ADEUTIL