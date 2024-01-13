#include "common/DisableStreamBuffer.hpp"
#include "util/ADEUtil.hpp"
#include "util/Stopwatch.hpp"

#include <iostream>
#include <map>

struct Node
{
    int value;
    static const char* name() { return "Node"; }
};

int main()
{
    disableStdOutBuffer();
    ade::Graph graph;
    ade::TypedGraph<Node> typedGraph(graph);
    std::map<int, ade::NodeHandle> nodes;
    for(int i = 0; i < 16; ++i)
    {
        auto node = typedGraph.createNode();
        typedGraph.metadata(node).set(Node{i});
        nodes.emplace(i, node);
    }
#define L(c1,c2) graph.link(nodes[c1], nodes[c2])
    L(0, 1);
    L(1, 2);
    L(2, 3);
    L(2, 4);
    L(2, 5);
    L(3, 6);
    L(4, 7);
    L(6, 8);
    L(7, 9);
    L(8, 9);
    L(6, 10);
    L(10, 11);
    L(9, 12);
    L(12, 14);
    L(14, 15);
    L(11, 13);
    L(13, 15);
#undef L
    auto squashedGraph = YADAW::Util::squashGraph(graph);
    std::vector<std::pair<int, int>> squashResult;
    std::transform(squashedGraph.begin(), squashedGraph.end(),
        std::back_inserter(squashResult),
        [&typedGraph](const std::pair<ade::NodeHandle, ade::NodeHandle>& pair)
        {
            return std::pair<int, int>(
                typedGraph.metadata(pair.first).get<Node>().value,
                typedGraph.metadata(pair.second).get<Node>().value
            );
        }
    );
    std::vector<std::pair<int, int>> correctSquashResult = {
        {0, 2},
        {3, 6},
        {4, 7},
        {5, 5},
        {8, 8},
        {9, 14},
        {10, 13},
        {15, 15}
    };
    if(!std::is_permutation(correctSquashResult.begin(), correctSquashResult.end(), squashResult.begin()))
    {
        std::fprintf(stderr, "Wrong Answer\n");
        return -1;
    }
    for(const auto& [from, to]: squashedGraph)
    {
        std::printf("%d -> %d\n",
            typedGraph.metadata(from).get<Node>().value,
            typedGraph.metadata(to).get<Node>().value);
    }
    if(squashedGraph.size() != correctSquashResult.size())
    {
        std::fprintf(stderr, "Wrong Answer\n");
        return -1;
    }
    auto topoResult = YADAW::Util::topologicalSort(graph, squashedGraph);
    for(const auto& row: *topoResult)
    {
        const char sep[2][2] = {
            ",",
            "\0"
        };
        for(const auto& link: row)
        {
            const auto& [from, to] = link;
            std::printf("%d -> %d%s ",
                typedGraph.metadata(from).get<Node>().value,
                typedGraph.metadata(to).get<Node>().value,
                sep[&link == &row.back()]);
        }
        std::printf("\n");
    }
}