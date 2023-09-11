#if __linux__

#include "audio/backend/ALSABackend.hpp"
#include "native/linux/ALSABackendImpl.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Backend
{
ALSABackend::ALSABackend():
    pImpl_(std::make_unique<Impl>(this))
{}

ALSABackend::ALSABackend(ALSABackend&& rhs) noexcept:
    pImpl_(std::move(rhs.pImpl_))
{}

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

bool ALSABackend::initialize(std::uint32_t sampleRate, std::uint32_t frameCount)
{
    pImpl_->initialize(sampleRate, frameCount);
    return true;
}

bool ALSABackend::uninitialize()
{
    pImpl_->uninitialize();
    return true;
}

std::uint32_t ALSABackend::sampleRate() const
{
    return pImpl_->sampleRate();
}

std::uint32_t ALSABackend::frameCount() const
{
    return pImpl_->frameCount();
}

ALSABackend::ActivateDeviceResult
ALSABackend::setAudioDeviceActivated(bool isInput, std::uint32_t index, bool activated)
{
    return pImpl_->setAudioDeviceActivated(isInput, index, activated);
}

bool ALSABackend::isAudioDeviceActivated(bool isInput, std::uint32_t index) const
{
    return pImpl_->isAudioDeviceActivated(isInput, index);
}

std::uint32_t ALSABackend::channelCount(bool isInput, std::uint32_t index) const
{
    return pImpl_->channelCount(isInput, index);
}

bool ALSABackend::start(const Callback* callback)
{
    return pImpl_->start(callback);
}

bool ALSABackend::stop()
{
    return pImpl_->stop();
}

std::optional<std::uint32_t> findDeviceBySelector(const ALSABackend& backend, bool isInput, ALSADeviceSelector selector)
{
    if(isInput)
    {
        auto inputCount = backend.audioInputDeviceCount();
        FOR_RANGE0(i, inputCount)
        {
            if(selector == backend.audioInputDeviceAt(i))
            {
                return i;
            }
        }
    }
    else
    {
        auto outputCount = backend.audioOutputDeviceCount();
        FOR_RANGE0(i, outputCount)
        {
            if(selector == backend.audioOutputDeviceAt(i))
            {
                return i;
            }
        }
    }
    return std::nullopt;
}
}

#endif