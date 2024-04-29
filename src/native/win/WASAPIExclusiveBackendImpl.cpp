#if _WIN32

#include "WASAPIExclusiveBackendImpl.hpp"

#include "util/IntegerRange.hpp"

// https://learn.microsoft.com/en-us/windows/win32/api/propkeydef/nf-propkeydef-define_propertykey#remarks
#include <initguid.h>

#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <Functiondiscoverykeys_devpkey.h>

#include <random>

enum class SampleFormat
{
    Float32,
    Int32,
    Int24,
    Int20,
    Int16,
    Int8,
    None
};

constexpr std::uint32_t channelCount[] = {8U, 6U, 4U, 2U, 1U};

constexpr DWORD channelConfig[] = {
    SPEAKER_FRONT_LEFT
  | SPEAKER_FRONT_RIGHT
  | SPEAKER_FRONT_CENTER
  | SPEAKER_FRONT_LEFT_OF_CENTER
  | SPEAKER_FRONT_RIGHT_OF_CENTER
  | SPEAKER_BACK_LEFT
  | SPEAKER_BACK_RIGHT
  | SPEAKER_LOW_FREQUENCY,
    SPEAKER_FRONT_LEFT
  | SPEAKER_FRONT_RIGHT
  | SPEAKER_FRONT_CENTER
  | SPEAKER_FRONT_LEFT_OF_CENTER
  | SPEAKER_FRONT_RIGHT_OF_CENTER
  | SPEAKER_LOW_FREQUENCY,
    SPEAKER_FRONT_LEFT
  | SPEAKER_FRONT_RIGHT
  | SPEAKER_BACK_LEFT
  | SPEAKER_BACK_RIGHT,
    SPEAKER_FRONT_LEFT
  | SPEAKER_FRONT_RIGHT,
  SPEAKER_FRONT_CENTER
};

namespace YADAW::Audio::Backend
{
namespace Impl
{
SampleFormat getHighestFormatSupported(IAudioClient& client)
{
return SampleFormat::None;
}
}

WASAPIExclusiveBackend::Impl::Impl(std::uint32_t sampleRate, std::uint32_t frameCount):
    sampleRate_(sampleRate), frameCount_(frameCount)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    auto createDeviceEnumeratorResult = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&deviceEnumerator_)
    );
    if(createDeviceEnumeratorResult != S_OK)
    {
        //
    }
    auto enumAudioInputDeviceResult = deviceEnumerator_->EnumAudioEndpoints(
        EDataFlow::eCapture, DEVICE_STATE_ACTIVE,
        &inputDeviceCollection_
    );
    if(enumAudioInputDeviceResult != S_OK)
    {
        //
    }
    auto enumAudioOutputDeviceResult = deviceEnumerator_->EnumAudioEndpoints(
        EDataFlow::eRender, DEVICE_STATE_ACTIVE,
        &outputDeviceCollection_
    );
    if(enumAudioOutputDeviceResult != S_OK)
    {
        //
    }
    UINT audioInputDeviceCount = 0;
    inputDeviceCollection_->GetCount(&audioInputDeviceCount);
    inputDevices_.resize(audioInputDeviceCount, nullptr);
    inputClients_.resize(audioInputDeviceCount, nullptr);
    captureClients_.resize(audioInputDeviceCount, nullptr);
    inputDeviceNames_.resize(audioInputDeviceCount);
    inputDeviceIDs_.resize(audioInputDeviceCount);
    FOR_RANGE0(i, audioInputDeviceCount)
    {
        IPropertyStore* propertyStore = nullptr;
        inputDeviceCollection_->Item(i, inputDevices_.data() + i);
        LPWSTR idString = nullptr;
        inputDevices_[i]->GetId(&idString);
        inputDeviceIDs_[i] = QString::fromWCharArray(idString);
        inputDevices_[i]->OpenPropertyStore(
            STGM_READ, &propertyStore
        );
        PROPVARIANT name;
        PropVariantInit(&name);
        propertyStore->GetValue(
            PKEY_Device_FriendlyName,
            &name
        );
        inputDeviceNames_[i] = QString::fromWCharArray(name.pwszVal);
        PropVariantClear(&name);
        propertyStore->Release();
    }
    UINT audioOutputDeviceCount = 0;
    outputDeviceCollection_->GetCount(&audioOutputDeviceCount);
    outputDevices_.resize(audioOutputDeviceCount, nullptr);
    outputClients_.resize(audioOutputDeviceCount, nullptr);
    renderClients_.resize(audioOutputDeviceCount, nullptr);
    outputDeviceNames_.resize(audioOutputDeviceCount);
    outputDeviceIDs_.resize(audioOutputDeviceCount);
    FOR_RANGE0(i, audioOutputDeviceCount)
    {
        IPropertyStore* propertyStore = nullptr;
        outputDeviceCollection_->Item(i, outputDevices_.data() + i);
        LPWSTR idString = nullptr;
        outputDevices_[i]->GetId(&idString);
        outputDeviceIDs_[i] = QString::fromWCharArray(idString);
        outputDevices_[i]->OpenPropertyStore(
            STGM_READ, &propertyStore
        );
        PROPVARIANT variant;
        {
            PROPVARIANT& name = variant;
            PropVariantInit(&name);
            propertyStore->GetValue(
                PKEY_Device_FriendlyName,
                &name
            );
            outputDeviceNames_[i] = QString::fromWCharArray(variant.pwszVal);
            PropVariantClear(&name);
        }
        {
            PROPVARIANT& supportsEventDriven = variant;
            PropVariantInit(&supportsEventDriven);
            propertyStore->GetValue(
                PKEY_AudioEndpoint_Supports_EventDriven_Mode,
                &supportsEventDriven
            );
            bool isEventDriverSupported = supportsEventDriven.uintVal;
            PropVariantClear(&supportsEventDriven);
        }
        if(propertyStore)
        {
            propertyStore->Release();
        }
    }
}

WASAPIExclusiveBackend::Impl::~Impl()
{
    for(auto client: captureClients_)
    {
        if(client)
        {
            client->Release();
        }
    }
    for(auto client: renderClients_)
    {
        if(client)
        {
            client->Release();
        }
    }
    for(auto device: inputDevices_)
    {
        device->Release();
    }
    for(auto device: outputDevices_)
    {
        device->Release();
    }
    inputDeviceCollection_->Release();
    outputDeviceCollection_->Release();
    if(deviceEnumerator_)
    {
        deviceEnumerator_->Release();
    }
    CoUninitialize();
}

std::uint32_t WASAPIExclusiveBackend::Impl::inputDeviceCount() const
{
    return inputDevices_.size();
}

std::uint32_t WASAPIExclusiveBackend::Impl::outputDeviceCount() const
{
    return outputDevices_.size();
}

std::optional<QString> WASAPIExclusiveBackend::Impl::inputDeviceIdAt(
    std::uint32_t index) const
{
    if(index < inputDeviceIDs_.size())
    {
        return {inputDeviceIDs_[index]};
    }
    return std::nullopt;
}

std::optional<QString> WASAPIExclusiveBackend::Impl::outputDeviceIdAt(
    std::uint32_t index) const
{
    if(index < outputDeviceIDs_.size())
    {
        return {outputDeviceIDs_[index]};
    }
    return std::nullopt;
}

std::optional<QString> WASAPIExclusiveBackend::Impl::inputDeviceNameAt(
    std::uint32_t index) const
{
    if(index < inputDeviceNames_.size())
    {
        return {inputDeviceNames_[index]};
    }
    return std::nullopt;
}

std::optional<QString> WASAPIExclusiveBackend::Impl::outputDeviceNameAt(
    std::uint32_t index) const
{
    if(index < outputDeviceNames_.size())
    {
        return {outputDeviceNames_[index]};
    }
    return std::nullopt;
}

QString WASAPIExclusiveBackend::Impl::defaultInputDeviceID() const
{
    IMMDevice* device = nullptr;
    auto getDefaultResult = deviceEnumerator_->GetDefaultAudioEndpoint(
        EDataFlow::eCapture, ERole::eMultimedia, &device
    );
    if(getDefaultResult == E_NOTFOUND)
    {
        return {};
    }
    LPWSTR name = nullptr;
    device->GetId(&name);
    device->Release();
    auto ret = QString::fromWCharArray(name);
    CoTaskMemFree(name);
    return ret;
}

QString WASAPIExclusiveBackend::Impl::defaultOutputDeviceID() const
{
    IMMDevice* device = nullptr;
    auto getDefaultResult = deviceEnumerator_->GetDefaultAudioEndpoint(
        EDataFlow::eRender, ERole::eMultimedia, &device
    );
    if(getDefaultResult == E_NOTFOUND)
    {
        return {};
    }
    LPWSTR name = nullptr;
    device->GetId(&name);
    device->Release();
    auto ret = QString::fromWCharArray(name);
    CoTaskMemFree(name);
    return ret;
}

std::optional<std::uint32_t> WASAPIExclusiveBackend::Impl::findInputDeviceIndexById(
    const QString& id) const
{
    auto it = std::find(inputDeviceIDs_.begin(), inputDeviceIDs_.end(), id);
    if(it != inputDeviceIDs_.end())
    {
        return it - inputDeviceIDs_.begin();
    }
    return std::nullopt;
}

std::optional<std::uint32_t> WASAPIExclusiveBackend::Impl::findOutputDeviceIndexById(
    const QString& id) const
{
    auto it = std::find(outputDeviceIDs_.begin(), outputDeviceIDs_.end(), id);
    if(it != outputDeviceIDs_.end())
    {
        return it - outputDeviceIDs_.begin();
    }
    return std::nullopt;
}

std::optional<bool> WASAPIExclusiveBackend::Impl::isInputDeviceActivated(
    std::uint32_t index) const
{
    if(index < captureClients_.size())
    {
        return captureClients_[index];
    }
    return std::nullopt;
}

std::optional<bool> WASAPIExclusiveBackend::Impl::isOutputDeviceActivated(
    std::uint32_t index) const
{
    if(index < renderClients_.size())
    {
        return renderClients_[index];
    }
    return std::nullopt;
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::Impl::activateInputDevice(
    std::uint32_t index, bool activate)
{
    auto ret = E_INVALIDARG;
    if(index < inputDeviceCount())
    {
        if(activate)
        {
            ret = inputDevices_[index]->Activate(
                __uuidof(IAudioClient),
                CLSCTX_INPROC_SERVER,
                NULL,
                reinterpret_cast<void**>(inputClients_.data() + index)
            );
            if(ret == S_OK)
            {
                auto client = inputClients_[index];
                // RANT: Negotiating audio device config in WASAPI exclusive
                // mode is somewhat difficult.
                // - We cannot retrieve a suitable config for exclusive mode
                //   by simply calling `IAudioClient::GetMixFormat` since it
                //   returns the format used in **shared** mode, which might
                //   not be supported in exclusive mode. For instance, the
                //   sample format might be floating-point, which is not
                //   supported by most audio devices.
                // - For WASAPI, we have to provide sample rate, channel
                //   count and sample format correctly at the same time.
                //   Even worse, if we'd like to initialize the client in
                //   exclusive mode (like what we are doing now), and one or
                //   some of the parameters are not supported, then
                //   `IAudioClient::IsFormatSupported` simply returns
                //   `AUDCLNT_E_UNSUPPORTED_FORMAT` with **no closest match
                //   available**. In other words, we do not know WHICH one
                //   parameter is rejected by simply checking the return
                //   value or the config itself.
                //   - On the contrary, ALSA user-mode API (or asound for
                //     short) on Linux supports checking one hardware
                //     parameter at a time, and gives us ways to return
                //     minimum/maximum sample rate or channel count range.

                // For our scenario, we'd like to initialize the audio
                // client with the highest channel count and the best sample
                // format available (currently float > int32 > int24 > int20
                // > int16 > int8). The steps are as follows:
                // - Fetch the format used in shared mode by calling
                //   `IAudioClient::GetMixFormat`.
                // - Set channel count (and byte rate) and test if the
                //   channel count is supported in shared mode. Since the
                //   only thing we only tampered the channel count and leave
                //   the rest of the parameters unchanged, we can take the
                //   first channel count that passed as the highest one.
                // - Set sample format (and byte rate) and test if the
                //   sample format is supported in exclusive mode.
                // - Initialize the audio client with the given sample rate
                //   and frame count, and hope it doesn't fail. ;)

                WAVEFORMATEXTENSIBLE exclusiveFormat;
                WAVEFORMATEX* formatBase = nullptr;
                auto getMixFormatResult = client->GetMixFormat(&formatBase);
                if(getMixFormatResult == S_OK)
                {
                    auto& extensibleFormatBase = *reinterpret_cast<WAVEFORMATEXTENSIBLE*>(formatBase);
                    exclusiveFormat = extensibleFormatBase;
                    CoTaskMemFree(formatBase);
                    // Find maximum channel count
                    {
                        WAVEFORMATEXTENSIBLE testChannelCountFormat = extensibleFormatBase;
                        std::uint32_t maxChannelIndex = std::size(channelCount);
                        FOR_RANGE0(i, std::size(channelCount))
                        {
                            testChannelCountFormat.Format.nChannels = channelCount[i];
                            testChannelCountFormat.dwChannelMask = channelConfig[i];
                            testChannelCountFormat.Format.nAvgBytesPerSec =
                                (testChannelCountFormat.Format.wBitsPerSample >> 3)
                                * channelCount[i] * testChannelCountFormat.Format.nSamplesPerSec;
                            auto testChannelCountResult = client->IsFormatSupported(
                                AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE,
                                &(testChannelCountFormat.Format), nullptr
                            );
                            if(testChannelCountResult == S_OK)
                            {
                                maxChannelIndex = i;
                                break;
                            }
                        }
                        auto& useChannelConfig = maxChannelIndex == std::size(channelCount)? extensibleFormatBase: testChannelCountFormat;
                        exclusiveFormat.Format.nChannels = useChannelConfig.Format.nChannels;
                        exclusiveFormat.Format.nAvgBytesPerSec = useChannelConfig.Format.nAvgBytesPerSec;
                        exclusiveFormat.dwChannelMask = useChannelConfig.dwChannelMask;
                    }
                    // TODO: Find best sample format
                }
                auto period = static_cast<REFERENCE_TIME>(
                    static_cast<float>(frameCount_) * 10'000'000 / static_cast<float>(sampleRate_) + 0.5f
                );
                auto initializeClientResult = client->Initialize(
                    AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE,
                    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                    period, period,
                    &(exclusiveFormat.Format),
                    NULL
                );
                while(initializeClientResult != S_OK)
                {
                    initializeClientResult = client->Initialize(
                        AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE,
                        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                        period, period,
                        &(exclusiveFormat.Format),
                        NULL
                    );
                }
                client->GetService(__uuidof(IAudioCaptureClient),
                    reinterpret_cast<void**>(captureClients_.data() + index)
                );
            }
        }
        else
        {
            captureClients_[index]->Release();
            captureClients_[index] = nullptr;
            inputClients_[index]->Reset();
            inputClients_[index]->Release();
            inputClients_[index] = nullptr;
        }
    }
    return *reinterpret_cast<YADAW::Native::ErrorCodeType*>(&ret);
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::Impl::activateOutputDevice(
    std::uint32_t index, bool activate)
{
    return false;
}
}

#endif