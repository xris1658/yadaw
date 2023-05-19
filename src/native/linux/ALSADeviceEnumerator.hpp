#ifndef YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
#define YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR

#if(__linux__)

#include <cstdint>
#include <optional>

namespace YADAW::Native
{
class ALSADeviceEnumerator
{
public:
    struct DeviceSelector
    {
        DeviceSelector(std::uint32_t c, std::uint32_t d):
            cIndex(c), dIndex(d) {}
        std::uint32_t cIndex;
        std::uint32_t dIndex;
    };
public:
    static void enumerateDevices();
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::uint32_t midiDeviceCount();
    static std::optional<DeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<DeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    static std::optional<DeviceSelector> midiDeviceAt(std::uint32_t index);
};
}

#endif

#endif //YADAW_SRC_NATIVE_LINUX_ALSADEVICEENUMERATOR
