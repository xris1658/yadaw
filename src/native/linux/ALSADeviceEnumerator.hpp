#ifndef YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
#define YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR

#if(__linux__)

#include "audio/backend/ALSADeviceSelector.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace YADAW::Native
{
using YADAW::Audio::Backend::ALSADeviceSelector;
class ALSADeviceEnumerator
{
public:
    static void enumerateDevices();
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::uint32_t midiDeviceCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> midiDeviceAt(std::uint32_t index);
    static std::optional<std::string> audioDeviceName(ALSADeviceSelector selector);
    static std::optional<std::string> cardName(int cardIndex);
};
}

#endif

#endif //YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
