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
private:
    class Impl;
public:
    enum class SampleFormat
    {
        Float32,
        Int32,
        Int24,
        Int16,
        Int8
    };
    using ErrorCode = HRESULT;
public:
    WASAPIExclusiveBackend(std::uint32_t sampleRate, std::uint32_t frameCount);
    ~WASAPIExclusiveBackend();
public:
    std::uint32_t inputDeviceCount() const;
    std::uint32_t outputDeviceCount() const;
    std::optional<QString> inputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> inputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceIdAt(std::uint32_t index) const;
    QString defaultInputDeviceId() const;
    QString defaultOutputDeviceId() const;
    std::optional<bool> isInputDeviceActivated(std::uint32_t index) const;
    std::optional<bool> isOutputDeviceActivated(std::uint32_t index) const;
    ErrorCode activateInputDevice(std::uint32_t index, bool activate);
    ErrorCode activateOutputDevice(std::uint32_t index, bool activate);
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_WASAPIEXCLUSIVEBACKEND
