#include "MIDIInputDeviceImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"

namespace YADAW::MIDI
{
using namespace YADAW::Native;
std::size_t MIDIInputDevice::Impl::inputDeviceCount()
{
    return YADAW::Native::ALSADeviceEnumerator::midiDeviceCount();
}

std::optional<YADAW::MIDI::DeviceInfo>
    MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    return ALSADeviceEnumerator::midiDeviceAt(index);
}

MIDIInputDevice::Impl::Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id):
    device_(device)
{
    if(auto result = snd_seq_open(&seq_, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
        result != 0)
    {
        auto errorMessage = snd_strerror(result);
        return;
    }
    // TODO: connect to MIDI device
}

MIDIInputDevice::Impl::~Impl()
{
    if(seqPortId_ != -1)
    {
        snd_seq_delete_port(seq_, seqPortId_);
    }
    if(seq_)
    {
        snd_seq_close(seq_);
    }
}

void MIDIInputDevice::Impl::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    //
}

void MIDIInputDevice::Impl::stop()
{
    if(midiThread_.joinable())
    {
        midiThread_.join();
    }
    //
}
}