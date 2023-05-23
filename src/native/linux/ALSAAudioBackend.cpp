#include "audio/backend/ALSAAudioBackend.hpp"
#include "native/linux/ALSAAudioBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
ALSAAudioBackend::ALSAAudioBackend()
{}

ALSAAudioBackend::ALSAAudioBackend(ALSAAudioBackend&& rhs) noexcept
{
    pImpl_ = std::move(rhs.pImpl_);
}

ALSAAudioBackend::~ALSAAudioBackend() {}

std::uint32_t ALSAAudioBackend::audioInputDeviceCount()
{
    return Impl::audioInputCount();
}

std::uint32_t ALSAAudioBackend::audioOutputDeviceCount()
{
    return Impl::audioOutputCount();
}

std::optional<ALSADeviceSelector> ALSAAudioBackend::audioInputDeviceAt(std::uint32_t index)
{
    return Impl::audioInputDeviceAt(index);
}

std::optional<ALSADeviceSelector> ALSAAudioBackend::audioOutputDeviceAt(std::uint32_t index)
{
    return Impl::audioOutputDeviceAt(index);
}

bool ALSAAudioBackend::initialize(std::uint32_t sampleRate, std::uint32_t frameSize)
{
    if(pImpl_)
    {
        return false;
    }
    pImpl_ = std::make_unique<Impl>(sampleRate, frameSize);
    return true;
}

bool ALSAAudioBackend::uninitialize()
{
    if(pImpl_)
    {
        pImpl_.reset();
    }
    return true;
}

ALSAAudioBackend::ActivateDeviceResult
ALSAAudioBackend::setAudioInputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    return pImpl_?
        pImpl_->setAudioInputDeviceActivated(selector, activated):
        ActivateDeviceResult::Failed;
}

ALSAAudioBackend::ActivateDeviceResult
ALSAAudioBackend::setAudioOutputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    return pImpl_?
        pImpl_->setAudioOutputDeviceActivated(selector, activated):
        ActivateDeviceResult::Failed;
}

bool ALSAAudioBackend::isAudioInputDeviceActivated(ALSADeviceSelector selector)
{
    return pImpl_? pImpl_->isAudioInputDeviceActivated(selector): false;
}

bool ALSAAudioBackend::isAudioOutputDeviceActivated(ALSADeviceSelector selector)
{
    return pImpl_? pImpl_->isAudioOutputDeviceActivated(selector): false;
}

bool ALSAAudioBackend::start()
{
    return pImpl_? pImpl_->start(): false;
}

bool ALSAAudioBackend::stop()
{
    return pImpl_? pImpl_->stop(): false;
}
}