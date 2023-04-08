#include "midi/MIDIInputDevice.hpp"

#include "MIDIInputDeviceImpl.hpp"

namespace YADAW::MIDI
{
using MIDIInputDeviceInfo = MIDIInputDevice::MIDIInputDeviceInfo;
std::vector<MIDIInputDeviceInfo> MIDIInputDevice::enumerateDevices()
{
    return Impl::enumerateDevices();
}

MIDIInputDevice::MIDIInputDevice(const QString& id):
    pImpl_(std::make_unique<Impl>(id))
{
}

MIDIInputDevice::~MIDIInputDevice()
{
}

void MIDIInputDevice::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    return pImpl_->start(func);
}

void MIDIInputDevice::stop()
{
    return pImpl_->stop();
}
}