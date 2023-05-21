#ifndef YADAW_ALSAAUDIOBACKEND
#define YADAW_ALSAAUDIOBACKEND

#include "audio/backend/ALSADeviceSelector.hpp"

#include <cstdint>
#include <memory>

namespace YADAW::Audio::Backend
{
class ALSAAudioBackend
{
    class Impl;
public:
    enum ActivateDeviceResult
    {
        Failed,
        Success,
        AlreadyDone
    };
    struct DeviceSelector
    {
        std::uint32_t cIndex;
        std::uint32_t dIndex;
    };
    struct DeviceInfo
    {
        QString id;
        QString name;
    };
public:
    ALSAAudioBackend();
    ALSAAudioBackend(const ALSAAudioBackend&) = delete;
    ALSAAudioBackend(ALSAAudioBackend&& rhs) noexcept;
    ~ALSAAudioBackend();
public:
    bool initialize();
    bool uninitialize();
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    bool setAudioInputDeviceActivated(DeviceInfo id, bool activated);
    bool setAudioOutputDeviceActivated(DeviceInfo id, bool activated);
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif //YADAW_ALSAAUDIOBACKEND
