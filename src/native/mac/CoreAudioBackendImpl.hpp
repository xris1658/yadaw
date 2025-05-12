#ifndef YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL
#define YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL

#if __APPLE__

#include "audio/backend/CoreAudioBackend.hpp"

#include <../../System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h>

namespace YADAW::Audio::Backend
{
class CoreAudioBackend::Impl
{
public:
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::optional<DeviceInfo> audioInputDeviceAt(std::uint32_t index);
    static std::optional<DeviceInfo> audioOutputDeviceAt(std::uint32_t index);
    static AudioDeviceID defaultInputDevice();
    static AudioDeviceID defaultOutputDevice();
    static std::optional<double> deviceNominalSampleRate(
        bool isInput, AudioDeviceID deviceID);
    static std::optional<std::vector<SampleRateRange>> deviceAvailableNominalSampleRates(
        bool isInput, AudioDeviceID deviceID);
    static std::optional<const std::vector<std::uint32_t>> deviceAvailableChannelCounts(
        bool isInput, AudioDeviceID id);
};
}

#endif

#endif // YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL
