#include "audio/backend/ALSABackend.hpp"
#include "native/linux/ALSABackendImpl.hpp"

namespace YADAW::Audio::Backend
{
ALSABackend::ALSABackend()
{}

ALSABackend::ALSABackend(ALSABackend&& rhs) noexcept
{
    pImpl_ = std::move(rhs.pImpl_);
}

ALSABackend::~ALSABackend() {}

std::uint32_t ALSABackend::audioInputDeviceCount()
{
    return Impl::audioInputCount();
}

std::uint32_t ALSABackend::audioOutputDeviceCount()
{
    return Impl::audioOutputCount();
}

std::optional<ALSADeviceSelector> ALSABackend::audioInputDeviceAt(std::uint32_t index)
{
    return Impl::audioInputDeviceAt(index);
}

std::optional<ALSADeviceSelector> ALSABackend::audioOutputDeviceAt(std::uint32_t index)
{
    return Impl::audioOutputDeviceAt(index);
}

std::optional<std::string> ALSABackend::audioDeviceName(ALSADeviceSelector selector)
{
    return Impl::audioDeviceName(selector);
}

std::optional<std::string> ALSABackend::cardName(int cardIndex)
{
    return Impl::cardName(cardIndex);
}

bool ALSABackend::initialize(std::uint32_t sampleRate, std::uint32_t frameSize)
{
    if(pImpl_)
    {
        return false;
    }
    pImpl_ = std::make_unique<Impl>(sampleRate, frameSize);
    return true;
}

bool ALSABackend::uninitialize()
{
    if(pImpl_)
    {
        pImpl_.reset();
    }
    return true;
}

ALSABackend::ActivateDeviceResult
ALSABackend::setAudioInputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    return pImpl_.get()?
        pImpl_->setAudioInputDeviceActivated(selector, activated):
        ActivateDeviceResult::Failed;
}

ALSABackend::ActivateDeviceResult
ALSABackend::setAudioOutputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    return pImpl_?
        pImpl_->setAudioOutputDeviceActivated(selector, activated):
        ActivateDeviceResult::Failed;
}

bool ALSABackend::isAudioInputDeviceActivated(ALSADeviceSelector selector)
{
    return pImpl_? pImpl_->isAudioInputDeviceActivated(selector): false;
}

bool ALSABackend::isAudioOutputDeviceActivated(ALSADeviceSelector selector)
{
    return pImpl_? pImpl_->isAudioOutputDeviceActivated(selector): false;
}

bool ALSABackend::start()
{
    return pImpl_? pImpl_->start(): false;
}

bool ALSABackend::stop()
{
    return pImpl_? pImpl_->stop(): false;
}
}