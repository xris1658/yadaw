#include "midi/MIDIInputDevice.hpp"

#include <cstdio>

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