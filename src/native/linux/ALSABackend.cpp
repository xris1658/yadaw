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

QString getIOString(bool isInput)
{
    return isInput?
    QCoreApplication::translate(
        "ALSABackend",
        "input"
    ):
    QCoreApplication::translate(
        "ALSABackend",
        "output"
    );
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
        ret = QCoreApplication::translate("ALSABackend",
            "opening audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::FillHardwareConfigSpace:
        ret = QCoreApplication::translate("ALSABackend",
            "retrieving available hardware config of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleFormat:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting sample format of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleRate:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting sample rate of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetBufferSize:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting buffer size of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetPeriodSize:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting period size of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::GetChannelCount:
        ret = QCoreApplication::translate("ALSABackend",
            "getting available channel count of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetChannelCount:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting channel count of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetSampleAccess:
        ret = QCoreApplication::translate("ALSABackend",
            "getting available sample access of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::DetermineHardwareConfig:
        ret = QCoreApplication::translate("ALSABackend",
            "committing hardware config of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::GetSotfwareConfig:
        ret = QCoreApplication::translate("ALSABackend",
            "retrieving current software config of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetAvailMin:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting minimum available count of samples of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::SetStartThreshold:
        ret = QCoreApplication::translate("ALSABackend",
            "adjusting start threshold of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::DetermineSoftwareConfig:
        ret = QCoreApplication::translate("ALSABackend",
            "committing software config of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::AllocateBuffer:
        ret = QCoreApplication::translate("ALSABackend",
            "allocating buffer of audio %1 device %2");
        break;
    case ALSABackend::ActivateDeviceProcess::Finish:
        ret = QCoreApplication::translate("ALSABackend",
            "finishing activation of audio %1 device %2");
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
        return QCoreApplication::translate(
            "ALSABackend",
            "An error occurred while %1: %2"
        ).arg(
            getALSAProcessString(
                isInput, activateDeviceResult.first, QString::fromStdString(*optionalName)
            ),
            snd_strerror(activateDeviceResult.second)
        );
    }
    else
    {
        return QString(
            "Unable to find audio %1 device %2-%3"
        ).arg(
            getIOString(isInput),
            QString::number(selector.cIndex),
            QString::number(selector.dIndex)
        );
    }
}
}

#endif