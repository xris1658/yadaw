#ifndef YADAW_TEST_COMMON_MIDICALLBACK
#define YADAW_TEST_COMMON_MIDICALLBACK

#include "midi/Message.hpp"
#include "midi/MIDIInputDevice.hpp"

void midiCallback(const YADAW::MIDI::MIDIInputDevice& device, const YADAW::MIDI::Message& message);

#endif // YADAW_TEST_COMMON_MIDICALLBACK
