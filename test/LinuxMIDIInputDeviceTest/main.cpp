#include "midi/MIDIInputDevice.hpp"

void printTime(const YADAW::MIDI::MIDIInputDevice& device, const YADAW::MIDI::Message& message)
{
    using namespace YADAW::MIDI;
    auto header = reinterpret_cast<ChannelVoiceMessageHeader*>(message.data);
    std::printf("Channel: %hhu\n", header->channel());
    std::printf("   Type: %hhu\n", header->type());
    switch(header->type())
    {
    case NoteOffMessage::TypeId:
    {
        auto noteOff = reinterpret_cast<NoteOffMessage*>(message.data);
        std::printf("      Note: %hhu\n", noteOff->note);
        std::printf("  Velocity: %hhu\n", noteOff->velocity);
        break;
    }
    case NoteOnMessage::TypeId:
    {
        auto noteOn = reinterpret_cast<NoteOnMessage*>(message.data);
        std::printf("      Note: %hhu\n", noteOn->note);
        std::printf("  Velocity: %hhu\n", noteOn->velocity);
        break;
    }
    case PolyKeyPressureMessage::TypeId:
    {
        auto pkp = reinterpret_cast<PolyKeyPressureMessage*>(message.data);
        std::printf("      Note: %hhu\n", pkp->note);
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
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    auto inputCount = MIDIInputDevice::inputDeviceCount();
    std::printf("%lld input devices\n", inputCount);
    for(decltype(inputCount) i = 0; i < inputCount; ++i)
    {
        const auto& device = MIDIInputDevice::inputDeviceAt(i);
        auto name = device->name.toLocal8Bit();
        auto id = device->id;
        std::printf("  %lu: %s (ID: %u, %u)\n",
            i + 1,
            name.data(),
            id.clientId,
            id.portId
        );
    }
    if(inputCount == 0)
    {
        return 0;
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