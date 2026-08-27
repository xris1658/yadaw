#ifndef YADAW_SRC_UTIL_ALGORITHM
#define YADAW_SRC_UTIL_ALGORITHM

#include "util/IntegerRange.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <new>
#include <optional>
#include <ranges>
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

template<typename Iterator1, typename Iterator2, typename PredicateType>
std::optional<std::vector<std::size_t>> permutationNotation(
    Iterator1 first1, Iterator1 last1,
    Iterator2 first2, PredicateType predicate)
{
    auto [mismatch1, mismatch2] = std::mismatch(first1, last1, first2, predicate);
    if(mismatch1 != last1)
    {
        auto last2 = std::next(first2, std::distance(first1, last1));
        std::vector<std::size_t> ret; ret.reserve(std::distance(first1, last1));
        std::ranges::copy(
            std::ranges::iota_view<std::size_t, std::size_t>(0, std::distance(first1, mismatch1)),
            std::back_inserter(ret)
        );
        std::vector<bool> searched(std::distance(mismatch1, last1), false);
        for(auto i = mismatch1; i != last1; ++i)
        {
            auto it = mismatch2;
            while(true)
            {
                it = std::find_if(
                    it, last2, [i](const auto& rhs) { return *i == rhs; }
                );
                if(it != last2)
                {
                    if(auto itSearch = std::next(searched.begin(), std::distance(mismatch2, it)); !*itSearch)
                    {
                        *itSearch = true;
                        ret.emplace_back(std::distance(first2, it));
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        if(std::ranges::all_of(searched, [](bool value) { return value; }))
        {
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        std::vector<std::size_t> ret; ret.reserve(std::distance(first1, last1));
        std::ranges::copy(
            std::ranges::iota_view<std::size_t, std::size_t>(0, std::distance(first1, last1)),
            std::back_inserter(ret)
        );
        return ret;
    }
}

template<typename Iterator1, typename Iterator2>
std::optional<std::vector<std::size_t>> permutationNotation(
    Iterator1 first1, Iterator1 last1, Iterator2 first2)
{
    return permutationNotation(
        first1, last1, first2,
        [](std::add_const_t<typename std::iterator_traits<Iterator1>::reference> lhs,
           std::add_const_t<typename std::iterator_traits<Iterator2>::reference> rhs)
        {
            return lhs == rhs;
        }
    );
}

template<typename Iterator1, typename Iterator2, typename PredicateType>
std::optional<std::vector<std::size_t>> permutationNotation(
    Iterator1 first1, Iterator1 last1,
    Iterator2 first2, Iterator2 last2, PredicateType predicate)
{
    return std::distance(first1, last1) == std::distance(first2, last2)?
        permutationNotation(first1, last1, first2, predicate):
        std::nullopt;
}

template<typename Iterator1, typename Iterator2>
std::optional<std::vector<std::size_t>> permutationNotation(
    Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
{
    return std::distance(first1, last1) == std::distance(first2, last2)?
        permutationNotation(first1, last1, first2):
        std::nullopt;
}
}

#endif // YADAW_SRC_UTIL_ALGORITHM
