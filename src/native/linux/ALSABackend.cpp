#if __linux__

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSAErrorText.hpp"
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

ALSABackend::ActivateDeviceResult ALSABackend::setAudioDeviceActivated(
    bool isInput, std::uint32_t index, bool activated)
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

std::optional<std::uint32_t> findDeviceBySelector(bool isInput, ALSADeviceSelector selector)
{
    if(isInput)
    {
        auto inputCount = ALSABackend::audioInputDeviceCount();
        FOR_RANGE0(i, inputCount)
        {
            if(selector == ALSABackend::audioInputDeviceAt(i))
            {
                return i;
            }
        }
    }
    else
    {
        auto outputCount = ALSABackend::audioOutputDeviceCount();
        FOR_RANGE0(i, outputCount)
        {
            if(selector == ALSABackend::audioOutputDeviceAt(i))
            {
                return i;
            }
        }
    }
    return std::nullopt;
}

// "opening audio device {}", process
QString getALSAProcessString(
    bool isInput,
    ALSABackend::ActivateDeviceProcess process,
    const QString& name)
{
    QString ret;
    switch(process)
    {
    case ALSABackend::ActivateDeviceProcess::OpenPCM:
        ret = getOpenPCMErrorText();
        break;
    case ALSABackend::ActivateDeviceProcess::FillHardwareConfigSpace:
        ret = getFillHardwareConfigSpaceText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleFormat:
        ret = getSetSampleFormatText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleRate:
        ret = getSetSampleRateText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetBufferSize:
        ret = getSetBufferSizeText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetPeriodSize:
        ret = getSetPeriodSizeText();
        break;
    case ALSABackend::ActivateDeviceProcess::GetChannelCount:
        ret = getGetChannelCountText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetChannelCount:
        ret = getSetChannelCountText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleAccess:
        ret = getSetSampleAccessText();
        break;
    case ALSABackend::ActivateDeviceProcess::DetermineHardwareConfig:
        ret = getDetermineHardwareConfigText();
        break;
    case ALSABackend::ActivateDeviceProcess::GetSotfwareConfig:
        ret = getGetSotfwareConfigText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetAvailMin:
        ret = getSetAvailMinText();
        break;
    case ALSABackend::ActivateDeviceProcess::SetStartThreshold:
        ret = getSetStartThresholdText();
        break;
    case ALSABackend::ActivateDeviceProcess::DetermineSoftwareConfig:
        ret = getDetermineSoftwareConfigText();
        break;
    case ALSABackend::ActivateDeviceProcess::AllocateBuffer:
        ret = getAllocateBufferText();
        break;
    case ALSABackend::ActivateDeviceProcess::Finish:
        ret = getFinishText();
        break;
    }
    ret = ret.arg(getIOString(isInput), name);
    return ret;
}

QString getALSAErrorString(
    bool isInput, ALSADeviceSelector selector,
    ALSABackend::ActivateDeviceResult activateDeviceResult)
{
    auto optionalName = ALSABackend::audioDeviceName(selector);
    if(optionalName.has_value())
    {
        return getALSAErrorText().arg(
            getALSAProcessString(
                isInput, activateDeviceResult.first, QString::fromStdString(*optionalName)
            ),
            snd_strerror(activateDeviceResult.second)
        );
    }
    else
    {
        return getALSADeviceNotFoundText().arg(
            getIOString(isInput),
            QString::number(selector.cIndex),
            QString::number(selector.dIndex)
        );
    }
}
}

#endif