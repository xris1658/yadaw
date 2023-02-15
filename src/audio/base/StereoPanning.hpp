#ifndef YADAW_SRC_AUDIO_BASE_STEREOPANNING
#define YADAW_SRC_AUDIO_BASE_STEREOPANNING

#include "src/util/Constants.hpp"

namespace YADAW::Audio::Base
{
enum class PanLaw
{
    Zero,
    ConstantPowerSine, // -3dB sin/cos taper
    ConstantPowerSqrt, // -3dB square-root taper
    ConstantPowerSineCompensate, // -3dB sin/cos taper with compensation
    ConstantPowerSqrtCompensate, // -3dB square-root taper with compensation
    ConstantGain // -6dB
};

struct StereoScale
{
    double left;
    double right;
};

template<PanLaw PL>
StereoScale scaleFromPanning(double panning);

template<>
StereoScale scaleFromPanning<PanLaw::Zero>(double panning)
{
    return panning <= 0?
           StereoScale{1.0, 1.0 + panning}:
           StereoScale{1.0 - panning, 1.0};
}

template<>
StereoScale scaleFromPanning<PanLaw::ConstantPowerSine>(double panning)
{
    if(panning == 0)
    {
        auto value = 0.5 * YADAW::Util::sqrt2<double>();
        return {value, value};
    }
    auto panningPolar = 0.25 * YADAW::Util::pi<double>() * (panning + 1);
    return {std::cos(panningPolar), std::sin(panningPolar)};
}

template<>
StereoScale scaleFromPanning<PanLaw::ConstantPowerSineCompensate>(double panning)
{
    if(panning == 0)
    {
        return {1.0, 1.0};
    }
    auto panningPolar = 0.25 * YADAW::Util::pi<double>() * (panning + 1);
    return {std::cos(panningPolar) * YADAW::Util::sqrt2<double>(),
        std::sin(panningPolar) * YADAW::Util::sqrt2<double>()};
}

template<>
StereoScale scaleFromPanning<PanLaw::ConstantPowerSqrt>(double panning)
{
    if(panning == 0)
    {
        auto value = 0.5 * YADAW::Util::sqrt2<double>();
        return {value, value};
    }
    return {std::sqrt(0.5 - 0.5 * panning), std::sqrt(0.5 + 0.5 * panning)};
}

template<>
StereoScale scaleFromPanning<PanLaw::ConstantPowerSqrtCompensate>(double panning)
{
    if(panning == 0)
    {
        return {1.0, 1.0};
    }
    return {std::sqrt(1.0 - 1.0 * panning), std::sqrt(1.0 + 1.0 * panning)};
}

template<>
StereoScale scaleFromPanning<PanLaw::ConstantGain>(double panning)
{
    return {0.5 - 0.5 * panning, 0.5 + 0.5 * panning};
}
}


#include <cmath>

#endif //YADAW_SRC_AUDIO_BASE_STEREOPANNING
