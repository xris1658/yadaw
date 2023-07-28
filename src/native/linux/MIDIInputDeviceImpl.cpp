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
    device_(device),
    deviceId_(id)
{
    auto seq = Sequencer::instance().seq();
    if(auto createPortResult = snd_seq_create_simple_port(
        seq, "YADAW application port",
        SND_SEQ_PORT_CAP_DUPLEX | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_MIDI_GENERIC);
        createPortResult >= 0)
    {
        seqPortId_ = createPortResult;
        snd_seq_addr_t sender;
        snd_seq_addr_t dest;
        sender.client = snd_seq_client_id(seq);
        sender.port = createPortResult;
        dest.client = deviceId_.clientId;
        dest.port = deviceId_.portId;
        snd_seq_port_subscribe_malloc(&subscription_);
        snd_seq_port_subscribe_set_sender(subscription_, &sender);
        snd_seq_port_subscribe_set_dest(subscription_, &dest);
        auto connectResult = snd_seq_subscribe_port(seq, subscription_);
        if(connectResult < 0)
        {
            snd_seq_port_subscribe_free(subscription_);
            subscription_ = nullptr;
        }
    }
}

MIDIInputDevice::Impl::~Impl()
{
    auto seq = Sequencer::instance().seq();
    if(subscription_)
    {
        snd_seq_unsubscribe_port(seq, subscription_);
        snd_seq_port_subscribe_free(subscription_);
        subscription_ = nullptr;
    }
    if(seqPortId_ != -1)
    {
        snd_seq_delete_port(seq, seqPortId_);
    }
}

void MIDIInputDevice::Impl::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
}

void MIDIInputDevice::Impl::stop()
{
}
}