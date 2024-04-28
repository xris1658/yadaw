#ifndef YADAW_SRC_AUDIO_BACKEND_WASAPIEXCLUSIVEBACKEND
#define YADAW_SRC_AUDIO_BACKEND_WASAPIEXCLUSIVEBACKEND

#if _WIN32

#include "native/Native.hpp"

#include <QString>

#include <memory>

namespace YADAW::Audio::Backend
{
class WASAPIExclusiveBackend
{
    class Impl;
    class DeviceInfo
    {
        QString name;
        QString id;
    };
public:
    WASAPIExclusiveBackend();
    ~WASAPIExclusiveBackend();
public:
    std::uint32_t audioInputDeviceCount() const;
    std::uint32_t audioOutputDeviceCount() const;
    std::optional<QString> audioInputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> audioOutputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> audioInputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> audioOutputDeviceIdAt(std::uint32_t index) const;
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_WASAPIEXCLUSIVEBACKEND
