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

AudioDeviceID CoreAudioBackend::defaultInputDevice()
{
    return Impl::defaultInputDevice();
}

AudioDeviceID CoreAudioBackend::defaultOutputDevice()
{
    return Impl::defaultOutputDevice();
}

std::optional<double> CoreAudioBackend::deviceNominalSampleRate(bool isInput, AudioDeviceID id)
{
    return Impl::deviceNominalSampleRate(isInput, id);
}

std::optional<std::vector<CoreAudioBackend::SampleRateRange>> CoreAudioBackend::deviceAvailableNominalSampleRates(bool isInput,
    AudioDeviceID id)
{
    return Impl::deviceAvailableNominalSampleRates(isInput, id);
}

std::optional<const std::vector<std::uint32_t>>
CoreAudioBackend::deviceAvailableChannelCounts(bool isInput, AudioDeviceID id)
{
    return Impl::deviceAvailableChannelCounts(isInput, id);
}
}

#endif