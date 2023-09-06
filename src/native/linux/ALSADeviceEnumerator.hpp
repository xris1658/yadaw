#ifndef YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
#define YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR

#if __linux__

#include "audio/backend/ALSADeviceSelector.hpp"
#include "midi/DeviceInfo.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace YADAW::Native
{
using YADAW::Audio::Backend::ALSADeviceSelector;
class ALSADeviceEnumerator
{
public:
    enum RescanFlags
    {
        AudioDevices = 1 << 0,
        MIDIDevices = 1 << 1
    };
public:
    static void enumerateDevices();
    static const std::vector<ALSADeviceSelector>& audioInputDevices();
    static const std::vector<ALSADeviceSelector>& audioOutputDevices();
    static const std::vector<YADAW::MIDI::DeviceInfo> midiInputDevices();
    static const std::vector<YADAW::MIDI::DeviceInfo> midiOutputDevices();
    static std::optional<std::string> audioDeviceName(ALSADeviceSelector selector);
    static std::optional<std::string> cardName(int cardIndex);
};
}

#endif

#endif // YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
