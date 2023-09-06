#if __linux__

#include "MIDIOutputDeviceImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIOutputDevice::Impl::outputDeviceCount()
{
    return YADAW::Native::ALSADeviceEnumerator::midiInputDevices().size();
}

std::optional<DeviceInfo> MIDIOutputDevice::Impl::outputDeviceAt(std::size_t index)
{
    return index < outputDeviceCount()?
        std::optional(YADAW::Native::ALSADeviceEnumerator::midiOutputDevices()[index]):
        std::nullopt;
}
}

#endif