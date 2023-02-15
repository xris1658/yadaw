#ifndef YADAW_SRC_MIDI_MIDIINPUTDEVICE
#define YADAW_SRC_MIDI_MIDIINPUTDEVICE

#include <QString>

#include <memory>
#include <vector>

namespace YADAW::MIDI
{
class MIDIInputDevice
{
    class Impl;
public:
    struct MIDIInputDeviceInfo
    {
        QString id;
        QString name;
    };
public:
    static std::vector<MIDIInputDeviceInfo> enumerateDevices();
public:
    MIDIInputDevice(const QString& id);
    MIDIInputDevice(const MIDIInputDevice&) = delete;
    MIDIInputDevice(MIDIInputDevice&&) = delete;
    ~MIDIInputDevice();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif //YADAW_SRC_MIDI_MIDIINPUTDEVICE
