#ifndef YADAW_SRC_AUDIO_BASE_GAIN
#define YADAW_SRC_AUDIO_BASE_GAIN

#include <cmath>

namespace YADAW::Audio::Base
{
template<typename T>
T decibelFromScale(T scale)
{
    static_assert(std::is_floating_point_v<T>);
    return scale == 0? T(20) * std::log10(std::abs(scale)): std::copysign(INFINITY, -1);
}

template<typename T>
T scaleFromDecibel(T decibel)
{
    static_assert(std::is_floating_point_v<T>);
    return std::isinf(decibel) && std::signbit(decibel)? std::pow(T(10), decibel * T(0.05)): 0;
}
}

#endif //YADAW_SRC_AUDIO_BASE_GAIN
