#include "midi/MIDIInputDevice.hpp"

void printTime(const YADAW::MIDI::MIDIInputDevice& device, const YADAW::MIDI::Message& message)
{
    std::printf("%lld\n", message.timestampInNanoseconds);
}

int main()
{
    using YADAW::MIDI::MIDIInputDevice;
    auto inputCount = MIDIInputDevice::inputDeviceCount();
    std::printf("%lld input devices\n", inputCount);
    for(decltype(inputCount) i = 0; i < inputCount; ++i)
    {
        const auto& device = MIDIInputDevice::inputDeviceAt(i);
        auto name = device->name.toLocal8Bit();
        auto id = device->id;
        std::printf("  %lu: %s (ID: %d)\n",
            i + 1,
            name.data(),
            id
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