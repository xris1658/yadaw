#ifndef YADAW_SRC_MIDI_MIDIINPUTDEVICE
#define YADAW_SRC_MIDI_MIDIINPUTDEVICE

#include "midi/DeviceInfo.hpp"
#include "midi/Message.hpp"

#include <QString>

#include <memory>
#include <optional>
#include <vector>

namespace YADAW::MIDI
{
class MIDIInputDevice
{
    class Impl;
public:
    using ReceiveInputFunc = void(const MIDIInputDevice& device, const YADAW::MIDI::Message& message);
public:
    static std::size_t inputDeviceCount();
    static std::optional<DeviceInfo> inputDeviceAt(std::size_t index);
public:
    MIDIInputDevice(const YADAW::Native::MIDIDeviceID& id);
    MIDIInputDevice(const MIDIInputDevice&) = delete;
    MIDIInputDevice(MIDIInputDevice&&) = delete;
    ~MIDIInputDevice();
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif // YADAW_SRC_MIDI_MIDIINPUTDEVICE
