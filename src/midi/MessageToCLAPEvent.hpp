#ifndef YADAW_SRC_MIDI_MESSAGETOCLAPEVENT
#define YADAW_SRC_MIDI_MESSAGETOCLAPEVENT

#include "midi/Message.hpp"

#include <clap/events.h>

namespace YADAW::MIDI
{
clap_event_header* createCLAPEventFromMessage(const YADAW::MIDI::Message& message);
}

#endif //YADAW_SRC_MIDI_MESSAGETOCLAPEVENT
