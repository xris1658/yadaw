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

// https://support.microsoft.com/windows/94991183-f69d-b4cf-4679-c98ca45f577a
QString getAccessDeniedErrorText()
{
    return QCoreApplication::translate(
        "AudioGraphErrorText",
        "<p>Access to the audio device is denied.</p>"
        "<p>If you encountered this error while activating audio input devices, go to "
        "<a href=\"ms-settings:privacy-microphone\"><b>Settings &gt; Privacy &gt; Microphone</b></a>.<br/>"
        "In <b>Allow access to the microphone on this device</b>, click <b>Change</b> "
        "and turn on <b>Microphone access for this device</b>. "
        "Then, turn on <b>Allow apps to access your microphone</b> "
        "and <b>Allow desktop apps to access your microphone</b>.</p>"
    );
}
}
