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
}