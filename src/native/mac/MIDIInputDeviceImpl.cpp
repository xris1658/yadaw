#include "MIDIInputDeviceImpl.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIInputDevice::Impl::inputDeviceCount()
{
    return 0;
}

std::optional<DeviceInfo> MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    return std::nullopt;
}

MIDIInputDevice::Impl::Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id)
{
}

MIDIInputDevice::Impl::~Impl()
{
}

void MIDIInputDevice::Impl::start(ReceiveInputFunc* const func)
{
}

void MIDIInputDevice::Impl::stop()
{
}
}
