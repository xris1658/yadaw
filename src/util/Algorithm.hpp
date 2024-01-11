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
}

#endif // YADAW_SRC_UTIL_ALGORITHM
