#if _WIN32

#include "WASAPIExclusiveBackendImpl.hpp"

#include "util/IntegerRange.hpp"

#include <functiondiscoverykeys_devpkey.h>

#include <combaseapi.h>
#include <audioclient.h>
#include <mmdeviceapi.h>

enum SampleFormat
{
    Float32,
    Int32,
    Int24,
    Int20,
    Int16,
    Int8,
    None
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

WASAPIExclusiveBackend::Impl::Impl()
{
    auto createDeviceEnumeratorResult = CoCreateInstance(
        __uuidof(IMMDeviceEnumerator),
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
    for(auto client: inputClients_)
    {
        if(client)
        {
        client->Release();
        }
    }
    for(auto client: outputClients_)
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
    if(index < inputClients_.size())
    {
        return inputClients_[index];
    }
    return std::nullopt;
}

std::optional<bool> WASAPIExclusiveBackend::Impl::isOutputDeviceActivated(
    std::uint32_t index) const
{
    if(index < outputClients_.size())
    {
        return outputClients_[index];
    }
    return std::nullopt;
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::Impl::activateInputDevice(
    std::uint32_t index, bool activate)
{
    if(index < inputDeviceCount())
    {
        if(activate)
        {
            IAudioClient* client = nullptr;
            auto activateDeviceResult = inputDevices_[index]->Activate(
                __uuidof(IAudioClient),
                CLSCTX_INPROC_SERVER,
                NULL,
                reinterpret_cast<void**>(&client)
            );
            if(activateDeviceResult == S_OK)
            {
                WAVEFORMATEXTENSIBLE exclusiveFormat;
                {
                    WAVEFORMATEX* formatBase = nullptr;
                    auto getMixFormatResult = client->GetMixFormat(&formatBase);
                    if(getMixFormatResult == S_OK)
                    {
                        exclusiveFormat = *reinterpret_cast<WAVEFORMATEXTENSIBLE*>(formatBase);
                        CoTaskMemFree(formatBase);
                    }
                }
                client->Initialize(
                    AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE,
                    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                    0, 0,
                    &(exclusiveFormat.Format),
                    NULL
                );
                client->GetService(__uuidof(IAudioCaptureClient),
                    reinterpret_cast<void**>(inputClients_.data() + index)
                );
            }
        }
        else
        {
            IAudioClient* client = nullptr;
            inputClients_[index]->QueryInterface(
                __uuidof(IAudioClient),
                reinterpret_cast<void**>(&client)
            );
            client->Release();
            inputClients_[index]->Release();
            inputClients_[index] = nullptr;
            client->Reset();
            client->Release();
        }
    }
    auto ret = E_INVALIDARG;
    return *reinterpret_cast<YADAW::Native::ErrorCodeType*>(&ret);
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::Impl::activateOutputDevice(
    std::uint32_t index, bool activate)
{
}
}

#endif