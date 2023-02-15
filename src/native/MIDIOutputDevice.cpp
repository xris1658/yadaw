#include "midi/MIDIOutputDevice.hpp"

#include "MIDIOutputDeviceImpl.hpp"

namespace YADAW::MIDI
{
using MIDIOutputDeviceInfo = MIDIOutputDevice::MIDIOutputDeviceInfo;
std::vector<MIDIOutputDeviceInfo> MIDIOutputDevice::enumerateDevices()
{
    return Impl::enumerateDevices();
}

MIDIOutputDevice::MIDIOutputDevice(const QString& id):
    pImpl_(std::make_unique<Impl>(id))
{
}

MIDIOutputDevice::~MIDIOutputDevice()
{
}
}