#if __APPLE__

#include "CoreAudioBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
std::uint32_t CoreAudioBackend::Impl::audioInputDeviceCount()
{
    AudioObjectPropertyAddress address {
        .mSelector = kAudioHardwarePropertyDevices,
        .mScope = kAudioObjectPropertyScopeInput,
        .mElement = kAudioObjectPropertyElementMain
    };
    std::uint32_t propertySize = 0;
    auto status = AudioObjectGetPropertyDataSize(
        static_cast<AudioObjectID>(kAudioObjectSystemObject),
        &address,
        0, nullptr, &propertySize
    );
    if(status != 0)
    {
        return 0;
    }
    return propertySize / sizeof(AudioDeviceID);
}

std::uint32_t CoreAudioBackend::Impl::audioOutputDeviceCount()
{
    AudioObjectPropertyAddress address {
        .mSelector = kAudioHardwarePropertyDevices,
        .mScope = kAudioObjectPropertyScopeOutput,
        .mElement = kAudioObjectPropertyElementMain
    };
    std::uint32_t propertySize = 0;
    auto status = AudioObjectGetPropertyDataSize(
        static_cast<AudioObjectID>(kAudioObjectSystemObject),
        &address,
        0, nullptr, &propertySize
    );
    if(status != 0)
    {
        return 0;
    }
    return propertySize / sizeof(AudioDeviceID);
}
}

#endif