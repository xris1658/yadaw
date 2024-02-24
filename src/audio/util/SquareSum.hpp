#ifndef YADAW_SRC_AUDIO_UTIL_SQUARESUM
#define YADAW_SRC_AUDIO_UTIL_SQUARESUM

#include <iterator>
#include <numeric>

namespace YADAW::Audio::Util
{
namespace Impl
{
template<typename T> struct SumSquare
{
    SumSquare(T value = T()): value_(value) {}
    T value_;
};

template<typename T>
T leftAddRightSquare(T lhs, T rhs)
{
    return lhs + rhs * rhs;
}

template<typename T>
struct ReduceSumSquareHelper
{
    SumSquare<T> operator()(SumSquare<T> lhs, SumSquare<T> rhs)
    {
        return {lhs.value_ + rhs.value_};
    }
    SumSquare<T> operator()(SumSquare<T> lhs, T rhs)
    {
        return {lhs.value_ + rhs * rhs};
    }
    SumSquare<T> operator()(T lhs, SumSquare<T> rhs)
    {
        return {lhs * lhs + rhs.value_};
    }
    SumSquare<T> operator()(T lhs, T rhs)
    {
        return {lhs * lhs + rhs * rhs};
    }
};
}

// Calculates square sum of a sequence of data.
// Given 16384 float samples between [-1.0, 1.0] (~300ms at 44100 Hz), the error
// of RMS calculated by coarse and precise versions is about -150 dB, which is
// precise enough in this scenario.
template<
    typename It,
    bool Precise = false,
    typename T = std::remove_const_t<typename std::iterator_traits<It>::value_type>
>
T squareSum(It first, It last)
{
    if constexpr(Precise)
    {
        return std::accumulate(first, last, &Impl::leftAddRightSquare<T>);
    }
    else
    {
        return std::reduce(first, last, Impl::SumSquare<T>(),
            Impl::ReduceSumSquareHelper<T>()).value_;
    }
}
}

#endif // YADAW_SRC_AUDIO_UTIL_SQUARESUM
