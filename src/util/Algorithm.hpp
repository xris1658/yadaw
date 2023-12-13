#ifndef YADAW_SRC_UTIL_ALGORITHM
#define YADAW_SRC_UTIL_ALGORITHM

#include "util/IntegerRange.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <new>
#include <utility>
#include <vector>

namespace YADAW::Util
{
template<typename Iterator, typename PredicateType>
void insertionSort(Iterator first, Iterator last, PredicateType predicate)
{
    if(first != last)
    {
        for(auto i = first; ++i != last;)
        {
            auto j = i;
            while(j != first)
            {
                auto k = j; --k;
                if(predicate(*j, *k))
                {
                    std::swap(*j, *k);
                }
                else
                {
                    break;
                }
                --j;
            }
        }
    }
}

template<typename Iterator>
void insertionSort(Iterator first, Iterator last)
{
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    insertionSort(first, last, std::less<ValueType>());
}

template<typename Iterator>
struct InPlaceMergeResult
{
    Iterator firstOfFirst;
    Iterator lastOfFirst;
    Iterator firstOfMiddle;
    Iterator lastOfMiddle;
};

// Do an in-place merge.
// Like the STL implementation, this function will allocate a temporary buffer to perform the merge.
// If the memory allocation failed, then the real in-place, inefficient merge operation is used.
// Returns the new position of elements on both sides of both sequences.
// If you don't need the position info, use `std::inplace_merge` from STL instead.
template<typename Iterator, typename PredicateType>
[[nodiscard("Use `std::inplace_merge` instead if return value of this is not used")]]
InPlaceMergeResult<Iterator> inPlaceMerge(Iterator first, Iterator middle, Iterator last, PredicateType predicate)
{
    InPlaceMergeResult<Iterator> ret = {last, last, last, last};
    auto& [fof, lof, fom, lom] = ret;
    auto count = last - first;
    if(count != 0)
    {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        auto* buffer = new(std::nothrow) ValueType[count];
        Iterator head[2] = {first, middle};
        if(buffer)
        {
            FOR_RANGE0(i, count)
            {
                Iterator toInsert = head[1];
                if(head[0] != middle && (
                    head[1] == last ||
                    predicate(*head[0], *head[1]) ||
                    (!predicate(*head[1], *head[0])))
                    ) // *iFirst <= *iSecond
                {
                    toInsert = head[0];
                    ++head[0];
                    if(fof == last)
                    {
                        fof = first + i;
                    }
                    if(toInsert == middle - 1)
                    {
                        lof = first + i;
                    }
                }
                else
                {
                    ++head[1];
                    if(fom == last)
                    {
                        fom = first + i;
                    }
                    if(toInsert == last - 1)
                    {
                        lom = first + i;
                    }
                }
                assert(toInsert != last);
                buffer[i] = std::move(*toInsert);
            }
            std::move(buffer, buffer + count, first);
            delete[] buffer;
        }
        else
        {
            FOR_RANGE0(i, count)
            {
                Iterator toInsert = head[1];
                if(head[0] != middle && (
                    head[1] == last ||
                    predicate(*head[0], *head[1]) ||
                    (!predicate(*head[1], *head[0])))
                    ) // *iFirst <= *iSecond
                {
                    toInsert = head[0];
                    ++head[0];
                    if(fof == last)
                    {
                        fof = first + i;
                    }
                    if(toInsert == middle - 1)
                    {
                        lof = first + i;
                    }
                }
                else
                {
                    std::rotate(head[0], head[1], head[1] + 1);
                    ++middle;
                    ++head[0];
                    ++head[1];
                    if(fom == last)
                    {
                        fom = first + i;
                    }
                    if(toInsert == last - 1)
                    {
                        lom = first + i;
                    }
                }
            }
        }
    }
    return ret;
}

// Do an in-place merge.
// Like the STL implementation, this function will allocate a temporary buffer to perform the merge.
// If the memory allocation failed, then the real in-place, inefficient merge operation is used.
// Returns the new position of elements on both sides of both sequences.
template<typename Iterator>
[[nodiscard("Use `std::inplace_merge` instead if return value of this is not used")]]
InPlaceMergeResult<Iterator> inPlaceMerge(Iterator first, Iterator middle, Iterator last)
{
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    return inPlaceMerge(first, middle, last, std::less<ValueType>());
}
}

#endif // YADAW_SRC_UTIL_ALGORITHM
