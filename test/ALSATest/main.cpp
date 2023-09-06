#include "audio/backend/ALSABackend.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "midi/MIDIOutputDevice.hpp"
#include "native/linux/ALSADeviceEnumerator.hpp"
#include "common/DisableStreamBuffer.hpp"

#include <cstdio>

int main(int argc, char** argv)
{
    disableStdOutBuffer();
    auto audioInputCount = YADAW::Audio::Backend::ALSABackend::audioInputDeviceCount();
    std::printf("Audio input device count: %u", audioInputCount);
    for(decltype(audioInputCount) i = 0; i < audioInputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioInputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto audioOutputCount = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceCount();
    std::printf("\n\nAudio output device count: %u", audioOutputCount);
    for(decltype(audioOutputCount) i = 0; i < audioOutputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto midiInputCount = YADAW::Native::ALSADeviceEnumerator::midiInputDevices().size();
    std::printf("\n\nMIDI input device count: %zu", midiInputCount);
    for(decltype(midiInputCount) i = 0; i < midiInputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiInputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    auto midiOutputCount = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices().size();
    std::printf("\n\nMIDI output device count: %zu", midiOutputCount);
    for(decltype(midiOutputCount) i = 0; i < midiOutputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    std::printf("\n\n");
    YADAW::Audio::Backend::ALSABackend backend;
    backend.initialize(44100, 512);
    YADAW::Audio::Backend::ALSADeviceSelector selector(0, 0);
    auto inputIndex = YADAW::Audio::Backend::findDeviceBySelector(backend, true, selector);
    auto outputIndex = findDeviceBySelector(backend, false, selector);
    if(inputIndex.has_value())
    {
        auto activateInputResult = backend.setAudioDeviceActivated(true, *inputIndex, true);
        if(activateInputResult == decltype(activateInputResult)::Failed)
        {
            std::printf("Activate default input failed!\n");
        }
    }
    if(outputIndex.has_value())
    {
        auto activateOutputResult = backend.setAudioDeviceActivated(false, *outputIndex, true);
        if(activateOutputResult == decltype(activateOutputResult)::Failed)
        {
            std::printf("Activate default output failed!\n");
        }
    }
    backend.start();
    getchar();
    backend.stop();
}
