#ifndef YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL

#if(__linux__)

#include "midi/MIDIInputDevice.hpp"

#include <alsa/asoundlib.h>

namespace YADAW::MIDI
{
class MIDIInputDevice::Impl
{
public:
    static std::size_t inputDeviceCount();
    static std::optional<MIDIInputDeviceInfo> inputDeviceAt(std::size_t index);
public:
    Impl(const MIDIInputDevice& device, const QString& id);
    ~Impl();
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
    const MIDIInputDevice& device_;
    snd_rawmidi_t* rawMIDI_ = nullptr;
    snd_seq_t* seq_ = nullptr;
    int seqPortId_ = -1;
};
}

#endif

#endif //YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL
