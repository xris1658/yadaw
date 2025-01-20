#include "audio/backend/CoreAudioBackend.hpp"

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
}