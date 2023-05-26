#ifndef YADAW_ALSAAUDIOBACKENDIMPL
#define YADAW_ALSAAUDIOBACKENDIMPL

#if(__linux__)

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSADeviceSelector.hpp"

#include <alsa/asoundlib.h>

#include <map>
#include <string>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSABackend::Impl
{
public:
    Impl(std::uint32_t sampleRate, std::uint32_t frameSize);
    ~Impl();
public:
    static std::uint32_t audioInputCount();
    static std::uint32_t audioOutputCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    ActivateDeviceResult setAudioInputDeviceActivated(ALSADeviceSelector selector, bool activated);
    ActivateDeviceResult setAudioOutputDeviceActivated(ALSADeviceSelector selector, bool activated);
    bool isAudioInputDeviceActivated(ALSADeviceSelector selector) const;
    bool isAudioOutputDeviceActivated(ALSADeviceSelector selector) const;
    bool start();
    bool stop();
private:
    std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t> activateDevice(bool isInput, ALSADeviceSelector selector);
private:
    std::uint32_t sampleRate_;
    std::uint32_t frameSize_;
    std::map<std::uint64_t, std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>> inputs_;
    std::map<std::uint64_t, std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>> outputs_;
};
}

#endif

#endif //YADAW_ALSAAUDIOBACKENDIMPL
