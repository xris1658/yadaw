#include "audio/backend/CoreAudioBackend.hpp"

#include "util/IntegerRange.hpp"

#include <QString>

#include <cinttypes>
#include <cstdint>
#include <cstdio>

int main()
{
    using YADAW::Audio::Backend::CoreAudioBackend;
    auto inputCount = CoreAudioBackend::audioInputDeviceCount();
    auto outputCount = CoreAudioBackend::audioOutputDeviceCount();
    std::printf("%" PRIu32" input devices\n", inputCount);
    std::printf("%" PRIu32" output devices\n", outputCount);
    std::printf("Input Devices:\n");
    auto defaultInput = CoreAudioBackend::defaultInputDevice();
    auto defaultOutput = CoreAudioBackend::defaultOutputDevice();
    char defaultIndicators[2] = {' ', '>'};
    FOR_RANGE0(i, inputCount)
    {
        auto [id, name] = *CoreAudioBackend::audioInputDeviceAt(i);
        auto localName = name.toLocal8Bit();
        std::printf("  %c #%" PRIu32": %s\n", defaultIndicators[id == defaultInput], i + 1, localName.data());
    }
    std::printf("Output Devices:\n");
    FOR_RANGE0(i, outputCount)
    {
        auto [id, name] = *CoreAudioBackend::audioOutputDeviceAt(i);
        auto localName = name.toLocal8Bit();
        std::printf("  %c #%" PRIu32": %s\n", defaultIndicators[id == defaultOutput], i + 1, localName.data());
    }
}