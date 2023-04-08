#include "MessageToVST3Event.hpp"

namespace YADAW::MIDI
{
using namespace Steinberg;
using namespace Steinberg::Vst;

Event createVST3EventFromMessage(const Message& message)
{
    Event ret {};
    auto data = message.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type)
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
            ret.noteOn.channel = noteOn->header.channel;
            ret.noteOn.noteId = -1;
            ret.noteOn.pitch = noteOn->note;
            ret.noteOn.velocity = noteOn->velocity / 127.0;
            ret.noteOn.length = 0;
            ret.noteOn.tuning = 0; // FIXME
            return ret;
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
            ret.noteOff.channel = noteOff->header.channel;
            ret.noteOff.noteId = -1;
            ret.noteOff.pitch = noteOff->note;
            ret.noteOff.velocity = noteOff->velocity / 127.0;
            ret.noteOff.tuning = 0; // FIXME
            return ret;
        }
        case YADAW::MIDI::PolyKeyPressureMessage::TypeId:
        {
            auto* polyKeyPressure = reinterpret_cast<const YADAW::MIDI::PolyKeyPressureMessage*>(data);
            ret.polyPressure.channel = polyKeyPressure->header.channel;
            ret.polyPressure.noteId = -1;
            ret.polyPressure.pitch = polyKeyPressure->note;
            ret.polyPressure.pressure = polyKeyPressure->pressure;
            return ret;
        }
        default:
            break;
        }
    }
    else
    {
        // TODO: Process other message type
    }
    return ret;
}
}