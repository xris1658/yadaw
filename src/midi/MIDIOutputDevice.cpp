#include "MIDIOutputDevice.hpp"
#include "native/MIDIOutputDeviceImpl.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIOutputDevice::outputDeviceCount()
{
    return Impl::outputDeviceCount();
}

std::optional<DeviceInfo> MIDIOutputDevice::outputDeviceAt(std::size_t index)
{
    return index < outputDeviceCount()?
        std::optional(Impl::outputDeviceAt(index)):
        std::nullopt;
}
}