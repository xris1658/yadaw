#ifndef YADAW_SRC_UTIL_ALGORITHM
#define YADAW_SRC_UTIL_ALGORITHM

#include <iterator>

namespace YADAW::Util
{
template<typename Iterator, typename PredicateType>
void insertionSort(Iterator first, Iterator last, PredicateType predicate)
{
    for (auto i = first; ++i != last;)
    {
        auto j = i;
        while (j != first)
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

#endif //YADAW_SRC_UTIL_ALGORITHM