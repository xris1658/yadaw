#include "MessageToCLAPEvent.hpp"

#include <cstdlib>

namespace YADAW::MIDI
{
clap_event_header* createCLAPEventFromMessage(const Message& message)
{
    auto data = message.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type)
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            auto* ret = reinterpret_cast<clap_event_note*>(std::malloc(sizeof(clap_event_note)));
            auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
            ret->channel = noteOn->header.channel;
            ret->note_id = -1;
            ret->key = noteOn->note;
            ret->velocity = noteOn->velocity / 127.0;
            return reinterpret_cast<clap_event_header*>(ret);
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            auto* ret = reinterpret_cast<clap_event_note*>(std::malloc(sizeof(clap_event_note)));
            auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
            ret->channel = noteOff->header.channel;
            ret->note_id = -1;
            ret->key = noteOff->note;
            ret->velocity = noteOff->velocity / 127.0;
            return reinterpret_cast<clap_event_header*>(ret);
        }
        default:
            break;
        }
    }
}
}