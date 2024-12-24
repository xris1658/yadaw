#ifndef YADAW_SRC_AUDIO_BASE_GAIN
#define YADAW_SRC_AUDIO_BASE_GAIN

#include <cmath>
#include <concepts>

namespace YADAW::Audio::Base
{
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
}

#endif // YADAW_SRC_AUDIO_BASE_GAIN
