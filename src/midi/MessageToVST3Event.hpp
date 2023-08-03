#ifndef YADAW_SRC_MIDI_MESSAGETOVST3EVENT
#define YADAW_SRC_MIDI_MESSAGETOVST3EVENT

#include "midi/Message.hpp"

#include <pluginterfaces/vst/ivstevents.h>

namespace YADAW::MIDI
{
bool fillVST3EventFromMessage(const YADAW::MIDI::Message& from, Steinberg::Vst::Event& to);

inline Steinberg::Vst::Event createVST3EventFromMessage(const YADAW::MIDI::Message& message)
{
    Steinberg::Vst::Event event;
    fillVST3EventFromMessage(message, event);
    return event;
}
}

#endif // YADAW_SRC_MIDI_MESSAGETOVST3EVENT
