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
    ade::Graph graph;
    ade::TypedGraph<Node> typedGraph(graph);
    std::map<int, ade::NodeHandle> nodes;
    for(int i = 0; i < 32; ++i)
    {
        auto node = typedGraph.createNode();
        typedGraph.metadata(node).set(Node{i});
        nodes.emplace(i, node);
    }
#define L(c1,c2) graph.link(nodes[c1], nodes[c2])
    L(0, 1);
    L(0, 13);
    L(0, 15);
    L(0, 21);
    L(0, 23);
    L(0, 24);
    L(0, 28);
    L(0, 5);
    L(0, 6);
    L(0, 8);
    L(1, 10);
    L(1, 11);
    L(1, 12);
    L(1, 16);
    L(1, 17);
    L(1, 18);
    L(1, 25);
    L(1, 26);
    L(1, 28);
    L(1, 5);
    L(1, 8);
    L(1, 9);
    L(2, 10);
    L(2, 11);
    L(2, 14);
    L(2, 15);
    L(2, 16);
    L(2, 18);
    L(2, 21);
    L(2, 22);
    L(2, 24);
    L(2, 25);
    L(2, 27);
    L(2, 4);
    L(3, 10);
    L(3, 12);
    L(3, 23);
    L(3, 24);
    L(3, 25);
    L(3, 28);
    L(3, 6);
    L(3, 7);
    L(3, 8);
    L(3, 9);
    L(4, 10);
    L(4, 11);
    L(4, 17);
    L(4, 20);
    L(4, 22);
    L(4, 23);
    L(4, 24);
    L(4, 26);
    L(4, 28);
    L(4, 30);
    L(4, 6);
    L(5, 10);
    L(5, 15);
    L(5, 18);
    L(5, 26);
    L(5, 29);
    L(5, 31);
    L(5, 8);
    L(6, 12);
    L(6, 15);
    L(6, 16);
    L(6, 18);
    L(6, 22);
    L(6, 24);
    L(6, 25);
    L(6, 30);
    L(6, 31);
    L(6, 9);
    L(7, 13);
    L(7, 16);
    L(7, 17);
    L(7, 25);
    L(7, 8);
    L(8, 11);
    L(8, 12);
    L(8, 13);
    L(8, 14);
    L(8, 15);
    L(8, 16);
    L(8, 17);
    L(8, 18);
    L(8, 20);
    L(8, 24);
    L(8, 26);
    L(8, 9);
    L(9, 12);
    L(9, 17);
    L(9, 21);
    L(9, 22);
    L(9, 25);
    L(9, 28);
    L(9, 29);
    L(10, 12);
    L(10, 16);
    L(10, 18);
    L(10, 19);
    L(10, 20);
    L(10, 21);
    L(10, 22);
    L(10, 25);
    L(10, 26);
    L(10, 29);
    L(10, 30);
    L(11, 20);
    L(11, 21);
    L(11, 23);
    L(11, 24);
    L(11, 25);
    L(11, 27);
    L(11, 29);
    L(12, 14);
    L(12, 15);
    L(12, 18);
    L(12, 24);
    L(12, 26);
    L(12, 27);
    L(12, 28);
    L(12, 29);
    L(12, 30);
    L(13, 14);
    L(13, 19);
    L(13, 20);
    L(13, 25);
    L(13, 27);
    L(13, 29);
    L(13, 30);
    L(13, 31);
    L(14, 18);
    L(14, 19);
    L(14, 21);
    L(14, 22);
    L(14, 23);
    L(14, 24);
    L(14, 28);
    L(15, 16);
    L(15, 17);
    L(15, 26);
    L(15, 27);
    L(15, 28);
    L(16, 17);
    L(16, 21);
    L(16, 23);
    L(16, 28);
    L(16, 29);
    L(16, 31);
    L(17, 19);
    L(17, 21);
    L(17, 23);
    L(17, 24);
    L(17, 28);
    L(17, 29);
    L(18, 19);
    L(18, 21);
    L(18, 23);
    L(18, 27);
    L(18, 30);
    L(18, 31);
    L(19, 26);
    L(19, 27);
    L(20, 26);
    L(20, 27);
    L(21, 22);
    L(21, 25);
    L(21, 29);
    L(21, 30);
    L(22, 25);
    L(22, 31);
    L(23, 24);
    L(23, 26);
    L(23, 27);
    L(23, 28);
    L(23, 29);
    L(23, 31);
    L(24, 25);
    L(24, 29);
    L(25, 27);
    L(26, 27);
    L(26, 29);
    L(27, 29);
    L(28, 30);
    L(28, 31);
    L(30, 31);
#undef L
    auto [sequence, time] = YADAW::Util::stopwatch(&YADAW::Util::topologicalOrder<Node>, std::ref(typedGraph));
    std::cout << "Unoptimized: " << time << '\n';
    if(sequence.has_value())
    {
        auto seq = sequence.value();
        for(const auto& points: seq)
        {
            for(const auto& point: points)
            {
                std::cout << point.value << ' ';
            }
            std::cout << '\n';
        }
    }
    std::cout << "Nodes: ";
    auto nodesInGraph = typedGraph.nodes();
    for(const auto& node: nodesInGraph)
    {
        std::cout << typedGraph.metadata(node).get<Node>().value << ' ';
    }
}