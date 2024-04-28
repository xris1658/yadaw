#ifndef YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL
#define YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL

#include "audio/backend/WASAPIExclusiveBackend.hpp"

#include <guiddef.h>

class IMMDeviceEnumerator;
class IMMDeviceCollection;
class IMMDevice;
class IAudioCaptureClient;
class IAudioRenderClient;

#include <QString>

#include <vector>

namespace YADAW::Audio::Backend
{
class WASAPIExclusiveBackend::Impl
{
public:
    // YADAWWSPXCLSVSSN (YADAW WASAPI Exclusive Session)
    static constexpr GUID sessionGUID = {
        0x41444159,
        0x5757,
        0x5053,
        {
            0x58, 0x43, 0x4c, 0x53, 0x56, 0x53, 0x53, 0x4e
        }
    };
public:
    Impl();
    ~Impl();
public:
    std::uint32_t inputDeviceCount() const;
    std::uint32_t outputDeviceCount() const;
    std::optional<QString> inputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceIdAt(std::uint32_t index) const;
    std::optional<QString> inputDeviceNameAt(std::uint32_t index) const;
    std::optional<QString> outputDeviceNameAt(std::uint32_t index) const;
    std::optional<std::uint32_t> findInputDeviceIndexById(const QString& id) const;
    std::optional<std::uint32_t> findOutputDeviceIndexById(const QString& id) const;
    std::optional<bool> isInputDeviceActivated(std::uint32_t index) const;
    std::optional<bool> isOutputDeviceActivated(std::uint32_t index) const;
    YADAW::Native::ErrorCodeType activateInputDevice(std::uint32_t index, bool activate);
    YADAW::Native::ErrorCodeType activateOutputDevice(std::uint32_t index, bool activate);
private:
    IMMDeviceEnumerator* deviceEnumerator_ = nullptr;
    IMMDeviceCollection* inputDeviceCollection_ = nullptr;
    IMMDeviceCollection* outputDeviceCollection_ = nullptr;
    std::vector<IMMDevice*> inputDevices_;
    std::vector<IMMDevice*> outputDevices_;
    std::vector<QString> inputDeviceNames_;
    std::vector<QString> outputDeviceNames_;
    std::vector<QString> inputDeviceIDs_;
    std::vector<QString> outputDeviceIDs_;
    std::vector<IAudioCaptureClient*> inputClients_;
    std::vector<IAudioRenderClient*> outputClients_;
};
}

#endif // YADAW_SRC_NATIVE_WIN_WASAPIEXCLUSIVEBACKENDIMPL
