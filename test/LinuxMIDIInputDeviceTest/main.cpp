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
        auto id = device->id.toLocal8Bit();
        std::printf("  %lu: %s (ID: %s)\n",
            i + 1,
            name.data(),
            id.data()
        );
    }
}