#ifndef YADAW_SRC_MIDI_DEVICEINFO
#define YADAW_SRC_MIDI_DEVICEINFO

#include "native/MIDIDeviceID.hpp"

#include <QString>

namespace YADAW::MIDI
{
struct DeviceInfo
{
    YADAW::Native::MIDIDeviceID id;
    QString name;
};
}

#endif //YADAW_SRC_MIDI_DEVICEINFO
