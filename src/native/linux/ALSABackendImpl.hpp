#ifndef YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
#define YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL

#if __linux__

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSADeviceSelector.hpp"

#include <alsa/asoundlib.h>

#include <optional>
#include <set>
#include <string>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSABackend::Impl
{
private:
    struct Comparison
    {
        using is_transparent = void;
        bool operator()(std::byte* lhs, std::byte* rhs) const
        {
            return lhs < rhs;
        }
        bool operator()(const std::shared_ptr<std::byte[]>& lhs, std::byte* rhs) const
        {
            return lhs.get() < rhs;
        }
        bool operator()(std::byte* lhs, const std::shared_ptr<std::byte[]>& rhs) const
        {
            return lhs < rhs.get();
        }
        bool operator()(const std::shared_ptr<std::byte[]>& lhs, const std::shared_ptr<std::byte[]>& rhs) const
        {
            return lhs.get() < rhs.get();
        }
    };
    using ContainerType = std::vector<std::tuple<ALSADeviceSelector, snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*>>;
    enum TupleElementType
    {
        DeviceSelector,
        PCMHandle,
        ChannelCount,
        Format,
        Access,
        Buffer
    };
public:
    Impl();
    ~Impl();
private:
    static bool compareTupleWithElement(ContainerType::const_reference elem, ALSADeviceSelector selector);
public:
    void initialize(std::uint32_t sampleRate, std::uint32_t frameSize);
    void uninitialize();
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
    std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*> activateDevice(bool isInput, ALSADeviceSelector selector);
    static std::shared_ptr<std::byte[]> allocateBuffer(std::uint32_t frameSize, std::uint32_t channelCount, std::uint32_t formatIndex);
private:
    std::uint32_t sampleRate_;
    std::uint32_t frameSize_;
    ContainerType inputs_;
    ContainerType outputs_;
    std::thread audioThread_;
    std::atomic_flag runFlag_ {ATOMIC_FLAG_INIT};
    std::set<std::shared_ptr<std::byte[]>, Comparison> buffers_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
