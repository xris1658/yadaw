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
    auto midiCount = YADAW::Native::ALSADeviceEnumerator::midiDeviceCount();
    std::printf("\n\nMIDI device count: %u", midiCount);
    for(decltype(midiCount) i = 0; i < midiCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiDeviceAt(i).value();
        std::printf("\n\t%u: hw:%u,%u", i + 1, value.cIndex, value.dIndex);
    }
    YADAW::Audio::Backend::ALSABackend backend;
    backend.initialize(44100, 512);
    YADAW::Audio::Backend::ALSADeviceSelector selector(0, 0);
    backend.setAudioInputDeviceActivated(selector, true);
    backend.setAudioOutputDeviceActivated(selector, true);
}