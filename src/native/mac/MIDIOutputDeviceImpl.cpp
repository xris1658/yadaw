#include "MIDIOutputDeviceImpl.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIOutputDevice::Impl::outputDeviceCount()
{
    return 0;
}

std::optional<DeviceInfo> MIDIOutputDevice::Impl::outputDeviceAt(std::size_t index)
{
    return std::nullopt;
}

MIDIOutputDevice::Impl::Impl(const MIDIOutputDevice& device, const YADAW::Native::MIDIDeviceID& id)
{
}

MIDIOutputDevice::Impl::~Impl()
{
}

void MIDIOutputDevice::Impl::start(SendToOutputFunc* const func)
{
}

void MIDIOutputDevice::Impl::stop()
{
}
}
