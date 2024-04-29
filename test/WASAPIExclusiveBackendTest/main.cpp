#include "audio/backend/WASAPIExclusiveBackend.hpp"

#include "util/IntegerRange.hpp"

#include <cstdio>

int main()
{
    YADAW::Audio::Backend::WASAPIExclusiveBackend backend(44100, 512);
    auto inputCount = backend.inputDeviceCount();
    std::printf("Audio Input Devices:\n");
    auto defaultInputID = backend.defaultInputDeviceId();
    std::uint32_t defaultInputIndex = 0;
    FOR_RANGE0(i, inputCount)
    {
        auto id   = *backend.inputDeviceIdAt(i);
        auto name = *backend.inputDeviceNameAt(i);
        if(id == defaultInputID)
        {
            defaultInputIndex = i;
            std::printf("  > ");
        }
        else
        {
            std::printf("    ");
        }
        std::printf("Device %u: %s (%s)\n",
            i + 1,
            name.toLocal8Bit().data(),
            id.toLocal8Bit().data()
        );
    }
    auto outputCount = backend.outputDeviceCount();
    std::printf("Audio Output Devices:\n");
    auto defaultOutputID = backend.defaultOutputDeviceId();
    std::uint32_t defaultOutputIndex = 0;
    FOR_RANGE0(i, outputCount)
    {
        auto id   = *backend.outputDeviceIdAt(i);
        auto name = *backend.outputDeviceNameAt(i);
        if(id == defaultOutputID)
        {
            defaultOutputIndex = i;
            std::printf("  > ");
        }
        else
        {
            std::printf("    ");
        }
        std::printf("Device %u: %s (%s)\n",
            i + 1,
            name.toLocal8Bit().data(),
            id.toLocal8Bit().data()
        );
    }
    backend.activateInputDevice(defaultInputIndex, true);
    backend.activateInputDevice(defaultOutputIndex, true);
    backend.activateInputDevice(defaultInputIndex, false);
    backend.activateInputDevice(defaultOutputIndex, false);
}