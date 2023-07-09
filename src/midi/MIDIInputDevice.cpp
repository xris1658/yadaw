#include "MIDIInputDevice.hpp"
#include "native/MIDIInputDeviceImpl.hpp"

namespace YADAW::MIDI
{
std::size_t MIDIInputDevice::inputDeviceCount()
{
    return Impl::inputDeviceCount();
}

std::optional<MIDIInputDevice::MIDIInputDeviceInfo> MIDIInputDevice::inputDeviceAt(std::size_t index)
{
    return Impl::inputDeviceAt(index);
}

MIDIInputDevice::MIDIInputDevice(const QString& id):
    pImpl_(std::make_unique<Impl>(id))
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