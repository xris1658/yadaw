#include "MessageToCLAPEvent.hpp"

#include <new>

namespace YADAW::MIDI
{
std::unique_ptr<std::byte[]> createCLAPEventFromMessage(const Message& message)
{
    auto data = message.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type())
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            if(auto* ret = new(std::nothrow) std::byte[sizeof(clap_event_note)]; ret)
            {
                auto retAsNoteOn = reinterpret_cast<clap_event_note*>(ret);
                retAsNoteOn->header.type = CLAP_EVENT_NOTE_ON;
                auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
                retAsNoteOn->channel = noteOn->header.channel();
                retAsNoteOn->note_id = -1;
                retAsNoteOn->key = noteOn->note;
                retAsNoteOn->velocity = noteOn->velocity / 127.0;
                return std::unique_ptr<std::byte[]>(ret);
            }
            break;
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            if(auto* ret = new(std::nothrow) std::byte[sizeof(clap_event_note)]; ret)
            {
                auto retAsNoteOff = reinterpret_cast<clap_event_note*>(ret);
                retAsNoteOff->header.type = CLAP_EVENT_NOTE_OFF;
                auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
                retAsNoteOff->channel = noteOff->header.channel();
                retAsNoteOff->note_id = -1;
                retAsNoteOff->key = noteOff->note;
                retAsNoteOff->velocity = noteOff->velocity / 127.0;
                return std::unique_ptr<std::byte[]>(ret);
            }
            break;
        }
        default:
            break;
        }
    }
    return nullptr;
}
}
