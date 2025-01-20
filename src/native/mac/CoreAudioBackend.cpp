#if __APPLE__

#include "audio/backend/CoreAudioBackend.hpp"
#include "native/mac/CoreAudioBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
std::uint32_t CoreAudioBackend::audioInputDeviceCount()
{
    return Impl::audioInputDeviceCount();
}

std::uint32_t CoreAudioBackend::audioOutputDeviceCount()
{
    return Impl::audioOutputDeviceCount();
}

std::optional<CoreAudioBackend::DeviceInfo> CoreAudioBackend::audioInputDeviceAt(std::uint32_t index)
{
    return Impl::audioInputDeviceAt(index);
}

std::optional<CoreAudioBackend::DeviceInfo> CoreAudioBackend::audioOutputDeviceAt(std::uint32_t index)
{
    return Impl::audioOutputDeviceAt(index);
}
}

#endif