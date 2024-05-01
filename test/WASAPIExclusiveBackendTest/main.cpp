#include <audioclient.h>

#include "audio/backend/WASAPIExclusiveBackend.hpp"

#include "util/IntegerRange.hpp"

#include <cstdio>

int main()
{
    YADAW::Audio::Backend::WASAPIExclusiveBackend backend(44100, 512);
    auto inputCount = backend.inputDeviceCount();
    std::printf("%u Audio Input Devices:\n", inputCount);
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
    std::printf("%u Audio Output Devices:\n", outputCount);
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
    auto activateInputDeviceResult = backend.activateInputDevice(defaultInputIndex, true);
    auto activateOutputDeviceResult = backend.activateOutputDevice(defaultOutputIndex, true);
    std::printf("0x%llx 0x%llx", activateInputDeviceResult, activateOutputDeviceResult);
    if(activateInputDeviceResult == AUDCLNT_E_SERVICE_NOT_RUNNING)
    {
        std::wprintf(L"Windows Audio Service is not running. Please start it by running\n"
                      "  `net start Audiosrv` as administrator.\n");
    }
}