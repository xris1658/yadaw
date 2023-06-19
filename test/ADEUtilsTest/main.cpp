#include "util/ADEUtil.hpp"
#include "util/Stopwatch.hpp"

#include <iostream>
#include <map>

struct Node
{
    char c;
    static const char* name() { return "Node"; }
};

int main()
{
    ade::Graph graph;
    ade::TypedGraph<Node> typedGraph(graph);
    char letters[] = "ABCDEFGHIJKLM";
    std::map<char, ade::NodeHandle> nodes;
    for(int i = 0; i < 13; ++i)
    {
        auto node = typedGraph.createNode();
        typedGraph.metadata(node).set(Node{letters[i]});
        nodes.emplace(letters[i], node);
    }
    // opencv/ade/tutorial/04_passes.cpp
    //
    //    A ---> B ---> C ---> D      L
    //           :             :      ^
    //           V             V      :
    //    F <--- E ---> G      H ---> I
    //    :      :             :      :
    //    :      `----> J <----'      :
    //    :             :             V
    //    '----> M <----'             K
#define L(c1,c2) graph.link(nodes[c1], nodes[c2])
    L('A', 'B');
    L('B', 'C');
    L('C', 'D');
    L('B', 'E');
    L('E', 'F');
    L('E', 'G');
    L('E', 'J');
    L('D', 'H');
    L('H', 'I');
    L('I', 'L');
    L('I', 'K');
    L('H', 'J');
    L('F', 'M');
    L('J', 'M');
#undef L
    auto [sequence, time] = YADAW::Util::stopwatch(&YADAW::Util::topologicalOrder<Node>, std::ref(typedGraph));
    std::cout << time << '\n';
    if(sequence.has_value())
    {
        auto seq = sequence.value();
        for(const auto& points: seq)
        {
            for(const auto& point: points)
            {
                std::putchar(point.c);
                std::putchar(' ');
            }
            std::putchar('\n');
        }
    }
    std::cout << "Nodes: ";
    auto nodesInGraph = typedGraph.nodes();
    for(const auto& node: nodesInGraph)
    {
        std::cout << typedGraph.metadata(node).get<Node>().c << ' ';
    }
}