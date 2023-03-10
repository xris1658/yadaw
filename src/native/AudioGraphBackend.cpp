#include "audio/backend/AudioGraphBackend.hpp"
#include "native/AudioGraphBackendImpl.hpp"
#include "native/winrt/QStringFromHString.hpp"

namespace YADAW::Audio::Backend
{
AudioGraphBackend::AudioGraphBackend(): pImpl_(std::make_unique<Impl>())
{
}

AudioGraphBackend::AudioGraphBackend(AudioGraphBackend&& rhs) noexcept
{
    swap(rhs);
}

AudioGraphBackend::~AudioGraphBackend()
{
    if(status_ == Status::Processing)
    {
        stop();
    }
    if(status_ == Status::Created)
    {
        destroyAudioGraph();
    }
}

int AudioGraphBackend::audioInputDeviceCount() const
{
    return pImpl_->audioInputDeviceCount();
}

int AudioGraphBackend::audioOutputDeviceCount() const
{
    return pImpl_->audioOutputDeviceCount();
}

AudioGraphBackend::DeviceInfo AudioGraphBackend::audioInputDeviceAt(int index) const
{
    using YADAW::Native::qStringFromHString;
    auto&& info = pImpl_->audioInputDeviceAt(index);
    return {qStringFromHString(info.Name()), qStringFromHString(info.Id()), info.IsEnabled()};
}

AudioGraphBackend::DeviceInfo AudioGraphBackend::audioOutputDeviceAt(int index) const
{
    using YADAW::Native::qStringFromHString;
    auto&& info = pImpl_->audioOutputDeviceAt(index);
    return {qStringFromHString(info.Name()), qStringFromHString(info.Id()), info.IsEnabled()};
}

bool AudioGraphBackend::createAudioGraph()
{
    if(pImpl_->createAudioGraph())
    {
        status_ = Status::Created;
        return true;
    }
    return false;
}

bool AudioGraphBackend::createAudioGraph(const QString& id)
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    auto async = DeviceInformation::CreateFromIdAsync(idAsHString);
    auto deviceInformation = async.get();
    if(deviceInformation && pImpl_->createAudioGraph(deviceInformation))
    {
        status_ = Status::Created;
        return true;
    }
    return false;
}

int AudioGraphBackend::enableDeviceInput(const QString& id)
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    auto async = DeviceInformation::CreateFromIdAsync(idAsHString);
    auto deviceInformation = async.get();
    if(deviceInformation)
    {
        return pImpl_->enableDeviceInput(deviceInformation);
    }
    return -1;
}

bool AudioGraphBackend::disableDeviceInput(int deviceInputIndex)
{
    return pImpl_->disableDeviceInput(deviceInputIndex);
}

AudioGraphBackend::DeviceInfo AudioGraphBackend::currentOutputDevice() const
{
    using YADAW::Native::qStringFromHString;
    const auto& info = pImpl_->currentOutputDevice();
    if(info)
    {
        return {qStringFromHString(info.Name()), qStringFromHString(info.Id()), info.IsEnabled()};
    }
    else
    {
        return {};
    }
}

void AudioGraphBackend::destroyAudioGraph()
{
    pImpl_->destroyAudioGraph();
    status_ = Status::Empty;
}

void AudioGraphBackend::start(AudioGraphBackend::AudioCallbackType* callback)
{
    pImpl_->start(callback);
    status_ = Status::Processing;
}

void AudioGraphBackend::stop()
{
    pImpl_->stop();
    status_ = Status::Created;
}

int YADAW::Audio::Backend::AudioGraphBackend::bufferSizeInFrames() const
{
    return pImpl_->bufferSizeInFrames();
}

int YADAW::Audio::Backend::AudioGraphBackend::latencyInSamples() const
{
    return pImpl_->latencyInSamples();
}

std::uint32_t YADAW::Audio::Backend::AudioGraphBackend::sampleRate() const
{
    return pImpl_->sampleRate();
}

void YADAW::Audio::Backend::AudioGraphBackend::swap(YADAW::Audio::Backend::AudioGraphBackend& rhs)
{
    std::swap(pImpl_, rhs.pImpl_);
    std::swap(status_, rhs.status_);
}
}