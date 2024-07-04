#include "ALSAErrorText.hpp"

#include <QCoreApplication>

namespace YADAW::Audio::Backend
{
QString getIOString(bool isInput)
{
    auto inputText = QCoreApplication::translate(
        "ALSAErrorText",
        "input"
    );
    auto outputText = QCoreApplication::translate(
        "ALSAErrorText",
        "output"
    );
    return isInput? inputText: outputText;
}

QString getOpenPCMErrorText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "opening audio %1 device %2"
    );
}

QString getFillHardwareConfigSpaceText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "retrieving available hardware config of audio %1 device %2"
    );
}

QString getSetSampleFormatText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting sample format of audio %1 device %2"
    );
}

QString getSetSampleRateText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting sample rate of audio %1 device %2"
    );
}

QString getSetBufferSizeText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting buffer size of audio %1 device %2"
    );
}

QString getSetPeriodSizeText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting period size of audio %1 device %2"
    );
}

QString getGetChannelCountText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "getting available channel count of audio %1 device %2"
    );
}

QString getSetChannelCountText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting channel count of audio %1 device %2"
    );
}

QString getSetSampleAccessText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "getting available sample access of audio %1 device %2"
    );
}

QString getDetermineHardwareConfigText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "committing hardware config of audio %1 device %2"
    );
}

QString getGetSotfwareConfigText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "retrieving current software config of audio %1 device %2"
    );
}

QString getSetAvailMinText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting minimum available count of samples of audio %1 device %2"
    );
}

QString getSetStartThresholdText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "adjusting start threshold of audio %1 device %2"
    );
}

QString getDetermineSoftwareConfigText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "committing software config of audio %1 device %2"
    );
}

QString getAllocateBufferText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "allocating buffer of audio %1 device %2"
    );
}

QString getFinishText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "finishing activation of audio %1 device %2"
    );
}

QString getALSAErrorText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "An error occurred while %1: %2"
    );
}

QString getALSADeviceNotFoundText()
{
    return QCoreApplication::translate(
        "ALSAErrorText",
        "Unable to find audio %1 device %2-%3"
    );
}
}
