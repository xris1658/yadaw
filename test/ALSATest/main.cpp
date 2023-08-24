#include "audio/backend/ALSABackend.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "native/linux/ALSADeviceEnumerator.hpp"

#include <cstdio>

int main(int argc, char** argv)
{
    auto audioInputCount = YADAW::Audio::Backend::ALSABackend::audioInputDeviceCount();
    std::printf("Audio input device count: %u", audioInputCount);
    for(decltype(audioInputCount) i = 0; i < audioInputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        std::printf("\n\t%u: %s (hw:%u,%u)", i + 1, name.c_str(), value.cIndex, value.dIndex);
    }
    auto audioOutputCount = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceCount();
    std::printf("\n\nAudio output device count: %u", audioOutputCount);
    for(decltype(audioOutputCount) i = 0; i < audioOutputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        std::printf("\n\t%u: %s (hw:%u,%u)", i + 1, name.c_str(), value.cIndex, value.dIndex);
    }
    auto midiInputCount = YADAW::Native::ALSADeviceEnumerator::midiInputDeviceCount();
    std::printf("\n\nMIDI device count: %u", midiInputCount);
    for(decltype(midiInputCount) i = 0; i < midiInputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiInputDeviceAt(i).value();
        std::printf("\n\t%u: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    auto midiOutputCount = YADAW::Native::ALSADeviceEnumerator::midiOutputDeviceCount();
    std::printf("\n\nMIDI device count: %u", midiOutputCount);
    for(decltype(midiOutputCount) i = 0; i < midiOutputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiOutputDeviceAt(i).value();
        std::printf("\n\t%u: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    YADAW::Audio::Backend::ALSABackend backend;
    backend.initialize(44100, 512);
    YADAW::Audio::Backend::ALSADeviceSelector selector(0, 0);
    backend.setAudioInputDeviceActivated(selector, true);
    backend.setAudioOutputDeviceActivated(selector, true);
}
