#include "MIDIInputDeviceImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"

namespace YADAW::MIDI
{
using namespace YADAW::Native;
std::size_t MIDIInputDevice::Impl::inputDeviceCount()
{
    return YADAW::Native::ALSADeviceEnumerator::midiDeviceCount();
}

std::optional<MIDIInputDevice::MIDIInputDeviceInfo>
    MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    auto info = ALSADeviceEnumerator::midiDeviceAt(index);
    if(info)
    {
        MIDIInputDeviceInfo ret;
        ret.id = QString("hw:%1,%2").arg(info->cIndex, info->dIndex);
        ret.name = QString("%1 (%2)").arg(
            QString::fromStdString(ALSADeviceEnumerator::audioDeviceName(info.value()).value()),
            QString::fromStdString(ALSADeviceEnumerator::cardName(info->cIndex).value())
        );
    }
}

MIDIInputDevice::Impl::Impl(const QString& id)
{
    auto idAs8Bit = id.toLocal8Bit();
    if(auto midiRet = snd_rawmidi_open(&rawMIDI_, nullptr, idAs8Bit.data(), SND_RAWMIDI_NONBLOCK);
        midiRet == 0)
    {
        if(auto seqRet = snd_seq_open(&seq_, "default", SND_SEQ_OPEN_INPUT, SND_SEQ_NONBLOCK);
            seqRet == 0)
        {
            if(auto portId = snd_seq_create_simple_port(
                    seq_,
                    "YADAW::MIDI::MIDIInputDeviceImpl sequencer port",
                    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
                    SND_SEQ_PORT_TYPE_MIDI_GENERIC
                );
                portId >= 0)
            {
                seqPortId_ = portId;
            }
        }
    }
}

MIDIInputDevice::Impl::~Impl()
{
    if(rawMIDI_)
    {
        snd_rawmidi_close(rawMIDI_);
    }
}

void MIDIInputDevice::Impl::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    // TODO
}

void MIDIInputDevice::Impl::stop()
{
    // TODO
}
}