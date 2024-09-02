#ifndef YADAW_SRC_AUDIO_BASE_GAIN
#define YADAW_SRC_AUDIO_BASE_GAIN

#include <cmath>
#if __cplusplus >= 202002L
#include <concepts>
#else
#include <type_traits>
#endif

namespace YADAW::Audio::Base
{
#if __cplusplus >= 202002L
template<std::floating_point T>
T decibelFromScale(T scale)
{
    return scale != 0? T(20) * std::log10(std::abs(scale)): std::copysign(INFINITY, -1);
}

template<std::floating_point T>
T scaleFromDecibel(T decibel)
{
    return std::isinf(decibel) && std::signbit(decibel)? 0: std::pow(T(10), decibel * T(0.05));
}
#else
template<typename  T>
T decibelFromScale(T scale)
{
    static_assert(std::is_floating_point_v<T>);
    return scale != 0? T(20) * std::log10(std::abs(scale)): std::copysign(INFINITY, -1);
}

template<typename T>
T scaleFromDecibel(T decibel)
{
    static_assert(std::is_floating_point_v<T>);
    return std::isinf(decibel) && std::signbit(decibel)? 0: std::pow(T(10), decibel * T(0.05));
}
#endif
}

#endif // YADAW_SRC_AUDIO_BASE_GAIN
