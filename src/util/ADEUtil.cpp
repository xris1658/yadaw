#include "ADEUtil.hpp"

#include <ade/handle.hpp>

#include <algorithm>
#include <unordered_set>

namespace YADAW::Util
{
using VisitedNodes = std::unordered_set<ade::NodeHandle, ade::HandleHasher<ade::Node>>;

bool pathExists(const ade::NodeHandle& from, const ade::NodeHandle& to, VisitedNodes& visitedNodes)
{
    auto outNodes = from->outNodes();
    return std::any_of(outNodes.begin(), outNodes.end(),
        [&to, &visitedNodes](const ade::NodeHandle& nodeHandle)
        {
            if(nodeHandle == to)
            {
                return true;
            }
            if(visitedNodes.find(nodeHandle) != visitedNodes.end())
            {
                return false;
            }
            else
            {
                visitedNodes.emplace(nodeHandle);
                return pathExists(nodeHandle, to, visitedNodes);
            }
        }
    );
}

bool pathExists(ade::NodeHandle from, ade::NodeHandle to)
{
    VisitedNodes visitedNodes;
    return pathExists(from, to, visitedNodes);
}

using Link = std::pair<ade::NodeHandle, ade::NodeHandle>;

std::vector<Link> squashGraph(const ade::Graph& graph)
{
    std::unordered_set<ade::NodeHandle, ade::HandleHasher<ade::Node>> linkStartNodes;
    std::unordered_set<ade::NodeHandle, ade::HandleHasher<ade::Node>> linkEndNodes;
    auto nodes = graph.nodes();
    std::for_each(nodes.begin(), nodes.end(),
        [&linkStartNodes, &linkEndNodes](const ade::NodeHandle& nodeHandle)
        {
            auto inNodes = nodeHandle->inNodes();
            auto outNodes = nodeHandle->outNodes();
            if(inNodes.empty() || inNodes.size() > 1)
            {
                linkStartNodes.emplace(nodeHandle);
                std::for_each(inNodes.begin(), inNodes.end(),
                    [&linkEndNodes](const ade::NodeHandle& inNode)
                    {
                        linkEndNodes.emplace(inNode);
                    }
                );
            }
            if(outNodes.empty() || outNodes.size() > 1)
            {
                linkEndNodes.emplace(nodeHandle);
                std::for_each(outNodes.begin(), outNodes.end(),
                    [&linkStartNodes](const ade::NodeHandle& outNode)
                    {
                        linkStartNodes.emplace(outNode);
                    }
                );
            }
        }
    );
    std::vector<Link> ret;
    std::for_each(linkStartNodes.begin(), linkStartNodes.end(),
        [&linkEndNodes, &ret](const ade::NodeHandle& nodeHandle)
        {
            auto i = nodeHandle;
            for(;
                linkEndNodes.find(i) == linkEndNodes.end();
                i = i->outNodes().front()
            )
            {}
            ret.emplace_back(nodeHandle, i);
        }
    );
    return ret;
}

std::optional<std::vector<std::vector<Link>>>
    topologicalSort(const std::vector<Link>& squashedGraph)
{
    std::vector<std::vector<Link>> ret;
    if(!squashedGraph.empty())
    {
        std::vector<Link> visitedLinks;
        std::vector<Link> unvisitedLinks = squashedGraph;
        while(!unvisitedLinks.empty())
        {
            auto partitionPoint = std::partition(
                unvisitedLinks.begin(), unvisitedLinks.end(),
                [&visitedLinks](const Link& link)
                {
                    auto inNodes = link.first->inNodes();
                    return !(inNodes.empty() || std::all_of(
                        inNodes.begin(), inNodes.end(),
                        [&visitedLinks](const ade::NodeHandle& nodeHandle)
                        {
                            return std::find_if(
                                visitedLinks.begin(), visitedLinks.end(),
                                [&nodeHandle](const Link& visitedLink)
                                {
                                    return visitedLink.second == nodeHandle;
                                }
                            ) != visitedLinks.end();
                        }
                    ));
                }
            );
            if(partitionPoint == unvisitedLinks.end())
            {
                return std::nullopt;
            }
            auto& currentPass = ret.emplace_back();
            std::copy(partitionPoint, unvisitedLinks.end(),
                std::back_inserter(visitedLinks));
            std::move(partitionPoint, unvisitedLinks.end(),
                std::back_inserter(currentPass));
            unvisitedLinks.erase(partitionPoint, unvisitedLinks.end());
        }
    }
    return {ret};
}
}