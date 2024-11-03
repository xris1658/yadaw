#if _WIN32

#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/backend/AudioGraphErrorText.hpp"
#include "native/win/AudioGraphBackendImpl.hpp"
#include "native/win/winrt/Async.hpp"
#include "native/win/winrt/QStringFromHString.hpp"
#include "util/IntegerRange.hpp"

#include <audioclient.h>

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
    uninitialize();
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
        pImpl_.reset();
        status_ = Status::Empty;
    }
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
    const auto& id = defaultAudioInputDeviceId();
    auto inputDeviceCount = audioInputDeviceCount();
    FOR_RANGE0(i, inputDeviceCount)
    {
        if(audioInputDeviceAt(i).id == id)
        {
            return i;
        }
    }
    return 0;
}

std::uint32_t AudioGraphBackend::defaultAudioOutputDeviceIndex() const
{
    const auto& id = defaultAudioOutputDeviceId();
    auto outputDeviceCount = audioOutputDeviceCount();
    FOR_RANGE0(i, outputDeviceCount)
    {
        if(audioOutputDeviceAt(i).id == id)
        {
            return i;
        }
    }
    return 0;
}

QString AudioGraphBackend::defaultAudioInputDeviceId() const
{
    return YADAW::Native::qStringFromHString(pImpl_->defaultAudioInputDeviceId());
}

QString AudioGraphBackend::defaultAudioOutputDeviceId() const
{
    return YADAW::Native::qStringFromHString(pImpl_->defaultAudioOutputDeviceId());
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

AudioGraphBackend::ErrorCode AudioGraphBackend::createAudioGraph(std::uint32_t sampleRate)
{
    AudioGraphBackend::ErrorCode ret = pImpl_->createAudioGraph(sampleRate);
    if(ret == 0)
    {
        status_ = Status::Created;
    }
    return ret;
}

AudioGraphBackend::ErrorCode AudioGraphBackend::createAudioGraph(const QString& id, std::uint32_t sampleRate)
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    auto async = DeviceInformation::CreateFromIdAsync(idAsHString);
    auto deviceInformation = asyncResult(async);
    auto ret = pImpl_->createAudioGraph(deviceInformation, sampleRate);
    if(ret == 0)
    {
        status_ = Status::Created;
    }
    return ret;
}

std::optional<std::uint32_t> AudioGraphBackend::findAudioInputDeviceById(const QString& id) const
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    return pImpl_->findAudioInputDeviceById(idAsHString);
}

bool AudioGraphBackend::isDeviceInputActivated(std::uint32_t deviceInputIndex) const
{
    return pImpl_->isDeviceInputActivated(deviceInputIndex);
}

AudioGraphBackend::ErrorCode AudioGraphBackend::activateDeviceInput(
    std::uint32_t deviceInputIndex, bool enabled)
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
    status_ = Status::Initialized;
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

std::uint32_t AudioGraphBackend::channelCount(bool isInput, std::uint32_t deviceIndex) const
{
    return pImpl_->channelCount(isInput, deviceIndex);
}

AudioGraphBackend::Status AudioGraphBackend::status() const
{
    return status_;
}

bool AudioGraphBackend::setOutputDeviceIndex(std::uint32_t index)
{
    if(index < audioOutputDeviceCount())
    {
        auto inputDeviceCount = audioInputDeviceCount();
        std::vector<bool> inputDeviceActivated(inputDeviceCount, false);
        FOR_RANGE0(i, inputDeviceCount)
        {
            inputDeviceActivated[i] = isDeviceInputActivated(i);
        }
        destroyAudioGraph();
        createAudioGraph(audioOutputDeviceAt(index).id);
        FOR_RANGE0(i, inputDeviceCount)
        {
            activateDeviceInput(i, inputDeviceActivated[i]);
        }
        return true;
    }
    return false;
}

void YADAW::Audio::Backend::AudioGraphBackend::swap(YADAW::Audio::Backend::AudioGraphBackend& rhs)
{
    std::swap(pImpl_, rhs.pImpl_);
    std::swap(status_, rhs.status_);
}

QString getAudioGraphErrorStringFromErrorCode(AudioGraphBackend::ErrorCode errorCode)
{
    switch(errorCode)
    {
    case AUDCLNT_E_DEVICE_IN_USE: // FIXME
        return getDeviceInUseErrorText();
    case E_ACCESSDENIED:
        return getAccessDeniedErrorText();
    default:
        break;
    }
    return QString();
}
}

#endif