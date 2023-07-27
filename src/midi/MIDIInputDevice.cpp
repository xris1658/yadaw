#include "MIDIInputDevice.hpp"
#include "native/MIDIInputDeviceImpl.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIInputDevice::inputDeviceCount()
{
    return Impl::inputDeviceCount();
}

std::optional<YADAW::MIDI::DeviceInfo> MIDIInputDevice::inputDeviceAt(std::size_t index)
{
    return Impl::inputDeviceAt(index);
}

MIDIInputDevice::MIDIInputDevice(const YADAW::Native::MIDIDeviceID& id):
    pImpl_(std::make_unique<Impl>(*this, id))
{}

MIDIInputDevice::~MIDIInputDevice()
{}

void MIDIInputDevice::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    pImpl_->start(func);
}

void MIDIInputDevice::stop()
{
    pImpl_->stop();
}
}