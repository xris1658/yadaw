#include "MessageToVST3Event.hpp"

namespace YADAW::MIDI
{
using namespace Steinberg;
using namespace Steinberg::Vst;

bool fillVST3EventFromMessage(const Message& from, Event& to)
{
    auto data = from.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type())
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            to.type = Event::EventTypes::kNoteOnEvent;
            auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
            to.noteOn.channel = noteOn->header.channel();
            to.noteOn.noteId = -1;
            to.noteOn.pitch = noteOn->note;
            to.noteOn.velocity = noteOn->velocity / 127.0;
            to.noteOn.length = 0;
            to.noteOn.tuning = 0; // FIXME
            return true;
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            to.type = Event::EventTypes::kNoteOffEvent;
            auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
            to.noteOff.channel = noteOff->header.channel();
            to.noteOff.noteId = -1;
            to.noteOff.pitch = noteOff->note;
            to.noteOff.velocity = noteOff->velocity / 127.0;
            to.noteOff.tuning = 0; // FIXME
            return true;
        }
        case YADAW::MIDI::PolyKeyPressureMessage::TypeId:
        {
            to.type = Event::EventTypes::kPolyPressureEvent;
            auto* polyKeyPressure = reinterpret_cast<const YADAW::MIDI::PolyKeyPressureMessage*>(data);
            to.polyPressure.channel = polyKeyPressure->header.channel();
            to.polyPressure.noteId = -1;
            to.polyPressure.pitch = polyKeyPressure->note;
            to.polyPressure.pressure = polyKeyPressure->pressure;
            return true;
        }
        default:
            break;
        }
    }
    else
    {
        // TODO: Process other message type
    }
    return false;
}
}