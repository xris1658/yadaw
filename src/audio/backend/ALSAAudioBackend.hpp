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
    bool initialize(std::uint32_t sampleRate, std::uint32_t frameSize);
    bool uninitialize();
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    ActivateDeviceResult setAudioInputDeviceActivated(ALSADeviceSelector selector, bool activated);
    ActivateDeviceResult setAudioOutputDeviceActivated(ALSADeviceSelector selector, bool activated);
    bool isAudioInputDeviceActivated(ALSADeviceSelector selector);
    bool isAudioOutputDeviceActivated(ALSADeviceSelector selector);
    bool start();
    bool stop();
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif //YADAW_ALSAAUDIOBACKEND