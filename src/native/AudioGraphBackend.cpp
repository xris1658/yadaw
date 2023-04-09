#include "audio/backend/AudioGraphBackend.hpp"
#include "native/AudioGraphBackendImpl.hpp"
#include "native/winrt/QStringFromHString.hpp"

namespace YADAW::Audio::Backend
{
AudioGraphBackend::AudioGraphBackend(): pImpl_()
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
    if(status_ == Status::Initialized)
    {
        uninitialize();
    }
}

bool AudioGraphBackend::initialize()
{
    try
    {
        pImpl_ = std::make_unique<Impl>();
        status_ = Status::Initialized;
        return true;
    }
    catch(...)
    {
        pImpl_.reset();
        return false;
    }
}

bool AudioGraphBackend::uninitialize()
{
    pImpl_.reset();
    status_ = Status::Empty;
    return true;
}

std::uint32_t AudioGraphBackend::audioInputDeviceCount() const
{
    return pImpl_->audioInputDeviceCount();
}

std::uint32_t AudioGraphBackend::audioOutputDeviceCount() const
{
    return pImpl_->audioOutputDeviceCount();
}

std::uint32_t AudioGraphBackend::defaultAudioInputDeviceIndex() const
{
    return pImpl_->defaultAudioInputDeviceIndex();
}

std::uint32_t AudioGraphBackend::defaultAudioOutputDeviceIndex() const
{
    return pImpl_->defaultAudioOutputDeviceIndex();
}

AudioGraphBackend::DeviceInfo AudioGraphBackend::audioInputDeviceAt(std::uint32_t index) const
{
    using YADAW::Native::qStringFromHString;
    auto&& info = pImpl_->audioInputDeviceAt(index);
    return {qStringFromHString(info.Name()), qStringFromHString(info.Id()), info.IsEnabled()};
}

AudioGraphBackend::DeviceInfo AudioGraphBackend::audioOutputDeviceAt(std::uint32_t index) const
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

bool AudioGraphBackend::isDeviceInputActivated(std::uint32_t deviceInputIndex) const
{
    return pImpl_->isDeviceInputActivated(deviceInputIndex);
}

AudioGraphBackend::DeviceInputResult
AudioGraphBackend::activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled)
{
    return pImpl_->activateDeviceInput(deviceInputIndex, enabled);
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