#ifndef YADAW_SRC_UTIL_ZECKENDORFLOGARITHM
#define YADAW_SRC_UTIL_ZECKENDORFLOGARITHM

#include "util/FibonacciIterator.hpp"

#include <optional>
#include <cmath>

namespace YADAW::Util
{
// https://keithschwarz.com/interesting/code/?dir=zeckendorf-logarithm
// Changes:
// - Add special cases (`n == 1` and `n == base`)
// - Assign rounded result to `prevExp` instead of floored result (which can be
//   0, in which case the function fails due to dividing numbers by zero)
// - Increment / decrement `fibBaseIterator` faster
template<typename IntegerType, typename T>
std::optional<IntegerType> zeckendorfLogarithm(const T& base, T n)
{
    if(n == 0 || base == 0 || base == 1)
    {
        return std::nullopt;
    }
    if(n == 1)
    {
        return {IntegerType(0)};
    }
    if(base == n)
    {
        return {IntegerType(1)};
    }
    auto fibExpIterator = std::pair(T(1), base);
    auto& [exp1, exp2] = fibExpIterator;
    auto fibBaseIterator = YADAW::Util::FibonacciIterator<IntegerType>::begin();
    while(exp2 < n)
    {
        T nextExp = exp1 * exp2;
        exp1 = exp2;
        exp2 = nextExp;
        ++fibBaseIterator;
    }
    auto result = IntegerType(0);
    while(fibBaseIterator->first != IntegerType(0) && exp1 != IntegerType(0))
    {
        if(exp1 <= n)
        {
            n /= exp1;
            result += fibBaseIterator->first;
        }
        T prevExp = std::round(double(exp2) / double(exp1));
        exp2 = exp1;
        exp1 = prevExp;

        --fibBaseIterator;
    }
    return {result};
}
}

#endif // YADAW_SRC_UTIL_ZECKENDORFLOGARITHM
