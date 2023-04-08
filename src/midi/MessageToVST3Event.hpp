#ifndef YADAW_SRC_MIDI_MESSAGETOVST3EVENT
#define YADAW_SRC_MIDI_MESSAGETOVST3EVENT

#include "midi/Message.hpp"

#include <pluginterfaces/vst/ivstevents.h>

namespace YADAW::MIDI
{
Steinberg::Vst::Event createVST3EventFromMessage(const YADAW::MIDI::Message& message);
}

#endif //YADAW_SRC_MIDI_MESSAGETOVST3EVENT
