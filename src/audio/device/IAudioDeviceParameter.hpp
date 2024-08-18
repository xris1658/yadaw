#ifndef YADAW_SRC_AUDIO_DEVICE_IPLUGINPARAMETER
#define YADAW_SRC_AUDIO_DEVICE_IPLUGINPARAMETER

#include "IParameter.hpp"

namespace YADAW::Audio::Device
{
class IAudioDeviceParameter
{
public:
    virtual ~IAudioDeviceParameter() {}
public:
    virtual std::uint32_t parameterCount() const = 0;
    virtual IParameter* parameter(std::uint32_t index) = 0;
    virtual const IParameter* parameter(std::uint32_t index) const = 0;
};
}

#endif // YADAW_SRC_AUDIO_DEVICE_IPLUGINPARAMETER
