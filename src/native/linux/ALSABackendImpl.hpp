#ifndef YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
#define YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL

#if __linux__

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSADeviceSelector.hpp"

#include <alsa/asoundlib.h>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSABackend::Impl
{
private:
    using ContainerType = std::vector<std::tuple<ALSADeviceSelector, snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>>;
    enum TupleElementType
    {
        DeviceSelector,
        PCMHandle,
        ChannelCount,
        Format,
        Access
    };
public:
    Impl(std::uint32_t sampleRate, std::uint32_t frameSize);
    ~Impl();
private:
    static bool compareTupleWithElement(ContainerType::const_reference elem, ALSADeviceSelector selector);
public:
    static std::uint32_t audioInputCount();
    static std::uint32_t audioOutputCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    static std::optional<std::string> audioDeviceName(ALSADeviceSelector selector);
    static std::optional<std::string> cardName(int cardIndex);
    ActivateDeviceResult setAudioDeviceActivated(bool isInput, std::uint32_t index, bool activated);
    bool isAudioDeviceActivated(bool isInput, std::uint32_t index) const;
    std::uint32_t channelCount(bool isInput, std::uint32_t index) const;
    bool start();
    bool stop();
private:
    std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t> activateDevice(bool isInput, ALSADeviceSelector selector);
private:
    static std::vector<ALSADeviceSelector> inputDevices_;
    static std::vector<ALSADeviceSelector> outputDevices_;
    std::uint32_t sampleRate_;
    std::uint32_t frameSize_;
    ContainerType inputs_;
    ContainerType outputs_;
    std::thread audioThread_;
    std::atomic_flag runFlag_ {ATOMIC_FLAG_INIT};
};
}

#endif

#endif // YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
