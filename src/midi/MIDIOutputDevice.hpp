#ifndef YADAW_SRC_MIDI_MIDIOUTPUTDEVICE
#define YADAW_SRC_MIDI_MIDIOUTPUTDEVICE

#include "midi/DeviceInfo.hpp"
#include "midi/Message.hpp"

#include <QString>

#include <memory>
#include <vector>

namespace YADAW::MIDI
{
class MIDIOutputDevice
{
    class Impl;
public:
    using SendToOutputFunc = void(const MIDIOutputDevice& device, const YADAW::MIDI::Message& message);
public:
    static std::size_t outputDeviceCount();
    static std::optional<DeviceInfo> outputDeviceAt(std::size_t index);
public:
    MIDIOutputDevice(const YADAW::Native::MIDIDeviceID& id);
    MIDIOutputDevice(const MIDIOutputDevice&) = delete;
    MIDIOutputDevice(MIDIOutputDevice&&) = delete;
    ~MIDIOutputDevice();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif // YADAW_SRC_MIDI_MIDIOUTPUTDEVICE
