#ifndef YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER

#include "audio/device/IDevice.hpp"

namespace YADAW::Controller
{
namespace Impl
{
template<typename T>
void process(YADAW::Audio::Device::IDevice* device,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    static_assert(std::is_base_of_v<YADAW::Audio::Device::IDevice, T>
        && (!std::is_same_v<YADAW::Audio::Device::IDevice, T>));
    static_cast<T*>(device)->process(audioProcessData);
}
}
}

#endif //YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
