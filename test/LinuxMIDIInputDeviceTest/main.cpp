#include "midi/MIDIInputDevice.hpp"

#include "test/common/MIDICallback.hpp"

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
    device.start(&midiCallback);
    getchar();
    device.stop();
}