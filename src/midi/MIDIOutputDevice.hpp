#ifndef YADAW_SRC_MIDI_MIDIOUTPUTDEVICE
#define YADAW_SRC_MIDI_MIDIOUTPUTDEVICE

#include <QString>

#include <memory>
#include <vector>

namespace YADAW::MIDI
{
class MIDIOutputDevice
{
    class Impl;

public:
    struct MIDIOutputDeviceInfo
    {
        QString id;
        QString name;
    };
public:
    MIDIOutputDevice(const QString& id);
    MIDIOutputDevice(const MIDIOutputDevice&) = delete;
    MIDIOutputDevice(MIDIOutputDevice&&) = delete;
    ~MIDIOutputDevice();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif // YADAW_SRC_MIDI_MIDIOUTPUTDEVICE
