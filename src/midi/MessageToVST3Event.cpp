#include "MessageToVST3Event.hpp"

namespace YADAW::MIDI
{
using namespace Steinberg;
using namespace Steinberg::Vst;

Event createVST3EventFromMessage(const Message& message)
{
    Event ret {};
    switch(message.data[0])
    {
    case 0x90:
        // TODO: channel
        ret.noteOn.noteId = -1;
        ret.noteOn.pitch = message.data[1];
        ret.noteOn.velocity = message.data[2] / 127.0;
        ret.noteOn.length = 0;
        break;
    case 0x80:
        // TODO: channel
        ret.noteOff.noteId = -1;
        ret.noteOff.pitch = message.data[1];
        ret.noteOff.velocity = message.data[2] / 127.0;
        break;
    default:
        break;
    }
    return ret;
}
}