#ifndef YADAW_SRC_MIDI_MESSAGETOCLAPEVENT
#define YADAW_SRC_MIDI_MESSAGETOCLAPEVENT

#include "midi/Message.hpp"

#include <clap/events.h>

#include <memory>

namespace YADAW::MIDI
{
std::unique_ptr<std::byte[]> createCLAPEventFromMessage(const YADAW::MIDI::Message& message);
}

#endif // YADAW_SRC_MIDI_MESSAGETOCLAPEVENT
