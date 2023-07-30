#include "MessageToCLAPEvent.hpp"

#include <new>

namespace YADAW::MIDI
{
std::unique_ptr<clap_event_header> createCLAPEventFromMessage(const Message& message)
{
    auto data = message.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type())
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            auto* ret = new(std::nothrow) clap_event_note();
            if(ret)
            {
                ret->header.type = CLAP_EVENT_NOTE_ON;
                auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
                ret->channel = noteOn->header.channel();
                ret->note_id = -1;
                ret->key = noteOn->note;
                ret->velocity = noteOn->velocity / 127.0;
            }
            return std::unique_ptr<clap_event_header>(reinterpret_cast<clap_event_header*>(ret));
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            auto* ret = new(std::nothrow) clap_event_note();
            if(ret)
            {
                ret->header.type = CLAP_EVENT_NOTE_OFF;
                auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
                ret->channel = noteOff->header.channel();
                ret->note_id = -1;
                ret->key = noteOff->note;
                ret->velocity = noteOff->velocity / 127.0;
            }
            return std::unique_ptr<clap_event_header>(reinterpret_cast<clap_event_header*>(ret));
        }
        default:
            break;
        }
    }
    return nullptr;
}
}
