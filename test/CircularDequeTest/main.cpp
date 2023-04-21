#include "util/Algorithm.hpp"
#include "util/CircularDeque.hpp"
#include "util/FixedSizeCircularDeque.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>

void circularDequeTest()
{
    YADAW::Util::CircularDeque<int> circularDeque(32);
    for(int i = 0; i < 32; ++i)
    {
        circularDeque.emplaceBack(i);
    }
    for(int i = 0; i < 32; ++i)
    {
        std::printf("%d, ", circularDeque[i]);
    }
    for(int i = 0; i < 16; ++i)
    {
        assert(circularDeque.popFront());
    }
    for(int i = 0; i < 16; ++i)
    {
        assert(circularDeque.emplaceBack(i + 32));
    }
    std::printf("\n");
    for(int i = 0; i < 32; ++i)
    {
        std::printf("%d, ", circularDeque[i]);
    }
    for(int i = 0; i < 32; ++i)
    {
        circularDeque.popFront();
    }
}

void fixedSizeCircularDequeTest()
{
    YADAW::Util::FixedSizeCircularDeque<int, 32> circularDeque;
    for(int i = 0; i < 32; ++i)
    {
        circularDeque.emplaceBack(i);
    }
    for(int i = 0; i < 32; ++i)
    {
        std::printf("%d, ", circularDeque[i]);
    }
    for(int i = 0; i < 16; ++i)
    {
        assert(circularDeque.popFront());
    }
    for(int i = 0; i < 16; ++i)
    {
        assert(circularDeque.emplaceBack(i + 32));
    }
    std::printf("\n");
    for(int i = 0; i < 32; ++i)
    {
        std::printf("%d, ", circularDeque[i]);
    }
    for(int i = 0; i < 32; ++i)
    {
        circularDeque.popFront();
    }
}

void insertionSortFSCQ()
{
    YADAW::Util::FixedSizeCircularDeque<int, 32> fscq;
    for(int i = 0; i < 16; ++i)
    {
        fscq.emplaceBack((i * 13) % 16);
    }
    YADAW::Util::insertionSort(fscq.begin(), fscq.end(), [](const int& a, const int& b) { return a < b; });
    assert(std::is_sorted(fscq.begin(), fscq.end()));
}

int main()
{
    circularDequeTest();
    std::printf("\n\n");
    fixedSizeCircularDequeTest();
    insertionSortFSCQ();
}