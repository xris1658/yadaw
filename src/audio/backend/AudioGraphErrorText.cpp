#include "AudioGraphErrorText.hpp"

#include <QCoreApplication>

namespace YADAW::Audio::Backend
{
QString getDeviceInUseErrorText()
{
    return QCoreApplication::translate(
        "AudioGraphErrorText",
        "The current audio device is already in use."
    );
}
}
