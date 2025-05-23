#ifndef YADAW_SRC_NATIVE_MIDIDEVICEID
#define YADAW_SRC_NATIVE_MIDIDEVICEID

#if _WIN32
#include <../include/winrt/base.h>
#elif __APPLE__
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>
#elif __linux__
#include <cstdint>
#endif

namespace YADAW::Native
{
#if _WIN32
using MIDIDeviceID = winrt::hstring;
#elif __APPLE__
using MIDIDeviceID = CFStringRef;
#elif __linux__
struct MIDIDeviceID
{
    std::uint32_t clientId;
    std::uint32_t portId;
    inline std::uint64_t key() const
    {
        return (static_cast<std::uint64_t>(clientId) << 32) + portId;
    }
    bool operator==(const MIDIDeviceID& rhs) const
    {
        return key() == rhs.key();
    }
    bool operator!=(const MIDIDeviceID& rhs) const
    {
        return key() != rhs.key();
    }
    bool operator<(const MIDIDeviceID& rhs) const
    {
        return key() < rhs.key();
    }
    bool operator>(const MIDIDeviceID& rhs) const
    {
        return key() > rhs.key();
    }
    bool operator<=(const MIDIDeviceID& rhs) const
    {
        return key() <= rhs.key();
    }
    bool operator>=(const MIDIDeviceID& rhs) const
    {
        return key() >= rhs.key();
    }
};
#endif
}

#endif // YADAW_SRC_NATIVE_MIDIDEVICEID
