#ifndef YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL
#define YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL

#include "audio/backend/WASAPIExclusiveBackend.hpp"

#include <guiddef.h>

class IMMDeviceEnumerator;
class IMMDeviceCollection;
class IMMDevice;
class IAudioClient;
class IAudioClock;
class IAudioCaptureClient;
class IAudioRenderClient;

#include <QString>

#include <atomic>
#include <thread>
#include <vector>

namespace YADAW::Audio::Backend
{
class WASAPIExclusiveBackend::Impl
{
public:
    Impl(std::uint32_t sampleRate, std::uint32_t frameCount);
    ~Impl();
public:
    std::uint32_t inputDeviceCount() const;
    std::uint32_t outputDeviceCount() const;
    std::optional<QString> inputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> inputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceNameAt(std::uint32_t index) const;
    QString defaultInputDeviceID() const;
    QString defaultOutputDeviceID() const;
    std::optional<std::uint32_t> findInputDeviceIndexById(const QString& id) const;
    std::optional<std::uint32_t> findOutputDeviceIndexById(const QString& id) const;
    std::optional<bool> isInputDeviceActivated(std::uint32_t index) const;
    std::optional<bool> isOutputDeviceActivated(std::uint32_t index) const;
    YADAW::Native::ErrorCodeType activateInputDevice(std::uint32_t index, bool activate);
    YADAW::Native::ErrorCodeType activateOutputDevice(std::uint32_t index, bool activate);
    void start();
    void stop();
private:
    YADAW::Native::ErrorCodeType activateDevice(bool isInput, std::uint32_t index, bool activate);
    void audioThread();
private:
    std::uint32_t sampleRate_;
    std::uint32_t frameCount_;
    IMMDeviceEnumerator* deviceEnumerator_ = nullptr;
    IMMDeviceCollection* inputDeviceCollection_ = nullptr;
    IMMDeviceCollection* outputDeviceCollection_ = nullptr;
    std::vector<IMMDevice*> inputDevices_;
    std::vector<IMMDevice*> outputDevices_;
    std::vector<QString> inputDeviceNames_;
    std::vector<QString> outputDeviceNames_;
    std::vector<QString> inputDeviceIDs_;
    std::vector<QString> outputDeviceIDs_;
    std::vector<IAudioClient*> inputClients_;
    std::vector<IAudioClient*> outputClients_;
    std::vector<IAudioCaptureClient*> captureClients_;
    std::vector<IAudioRenderClient*> renderClients_;
    std::vector<IAudioClock*> inputClocks_;
    std::vector<IAudioClock*> outputClocks_;
    std::thread audioThread_;
    std::atomic_bool audioThreadRunning_;
};
}

#endif // YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL
