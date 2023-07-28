#include "midi/MIDIInputDevice.hpp"
#include "util/Literal.hpp"

#include <cstdio>

void printTime(const YADAW::MIDI::MIDIInputDevice& device, const YADAW::MIDI::Message& message)
{
    using namespace YADAW::MIDI;
    using namespace YADAW::Util;
    auto header = reinterpret_cast<ChannelVoiceMessageHeader*>(message.data);
    std::printf("Channel: %hhu\n", header->channel());
    std::printf("   Type: %hhu\n", header->type());
    switch(header->type())
    {
    case NoteOffMessage::TypeId:
    {
        auto noteOff = reinterpret_cast<NoteOffMessage*>(message.data);
        auto note = noteOff->note;
        auto noteName = getNoteName<char>(note, MIDINoteNameKeyShift::kSharp, MIDINoteOctaveOffset::kNoOffset);
        std::printf("      Note: %s\n", noteName.data());
        std::printf("  Velocity: %hhu\n", noteOff->velocity);
        break;
    }
    case NoteOnMessage::TypeId:
    {
        auto noteOn = reinterpret_cast<NoteOnMessage*>(message.data);
        auto note = noteOn->note;
        auto noteName = getNoteName<char>(note, MIDINoteNameKeyShift::kSharp, MIDINoteOctaveOffset::kNoOffset);
        std::printf("      Note: %s\n", noteName.data());
        std::printf("  Velocity: %hhu\n", noteOn->velocity);
        break;
    }
    case PolyKeyPressureMessage::TypeId:
    {
        auto pkp = reinterpret_cast<PolyKeyPressureMessage*>(message.data);
        auto note = pkp->note;
        auto noteName = getNoteName<char>(note, MIDINoteNameKeyShift::kSharp, MIDINoteOctaveOffset::kNoOffset);
        std::printf("      Note: %s\n", noteName.data());
        std::printf("  Pressure: %hhu\n", pkp->pressure);
        break;
    }
    case ControlChangeMessage::TypeId:
    {
        auto cc = reinterpret_cast<ControlChangeMessage*>(message.data);
        std::printf("  Ctrl. ID: %hhu\n", cc->controlId);
        std::printf("     Value: %hhu\n", cc->controlValue);
        break;
    }
    case ProgramChangeMessage::TypeId:
    {
        auto pc = reinterpret_cast<ProgramChangeMessage*>(message.data);
        std::printf("  Prog. ID: %hhu\n", pc->programId);
        break;
    }
    case ChannelPressureMessage::TypeId:
    {
        auto cp = reinterpret_cast<ChannelPressureMessage*>(message.data);
        std::printf("  Pressure: %hhu\n", cp->pressure);
        break;
    }
    case PitchBendChangeMessage::TypeId:
    {
        auto pb = reinterpret_cast<PitchBendChangeMessage*>(message.data);
        std::printf("  PB Value: %hd\n", pb->value());
        break;
    }
    default:
        break;
    }
    std::printf("   Time: %lld\n", message.timestampInNanoseconds);
}

int main()
{
    using YADAW::MIDI::MIDIInputDevice;
    auto inputCount = MIDIInputDevice::inputDeviceCount();
    std::printf("%lld input devices\n", inputCount);
    for(decltype(inputCount) i = 0; i < inputCount; ++i)
    {
        const auto& device = MIDIInputDevice::inputDeviceAt(i);
        std::printf("  %lld: %ls (%ls)\n",
            i + 1,
            reinterpret_cast<const wchar_t*>(device->name.data()),
            reinterpret_cast<const wchar_t*>(device->id.data())
        );
    }
    int index = -1;
    while(true)
    {
        std::scanf("%d", &index);
        if(index > 0 && index <= inputCount)
        {
            break;
        }
    }
    getchar();
    YADAW::MIDI::MIDIInputDevice device(MIDIInputDevice::inputDeviceAt(index - 1)->id);
    device.start(&printTime);
    getchar();
    device.stop();
}