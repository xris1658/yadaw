#ifndef YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
#define YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL

#if __linux__

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSADeviceSelector.hpp"

#include <alsa/asoundlib.h>

#include <optional>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSABackend::Impl
{
private:
    template<typename T>
    struct Comparison
    {
        using is_transparent = void;
        bool operator()(T* lhs, T* rhs) const
        {
            return lhs < rhs;
        }
        bool operator()(const std::shared_ptr<T[]>& lhs, T* rhs) const
        {
            return lhs.get() < rhs;
        }
        bool operator()(T* lhs, const std::shared_ptr<T[]>& rhs) const
        {
            return lhs < rhs.get();
        }
        bool operator()(const std::shared_ptr<T[]>& lhs, const std::shared_ptr<T[]>& rhs) const
        {
            return lhs.get() < rhs.get();
        }
    };
    using DataType = std::tuple<ALSADeviceSelector, snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*, void**, snd_pcm_uframes_t, snd_pcm_uframes_t>;
    using RWFunc = snd_pcm_sframes_t(std::uint32_t, DataType&);
    static snd_pcm_sframes_t readMMapInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readMMapNonInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readNonInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeMMapInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeMMapNonInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeNonInterleavedBegin(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readMMapInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readMMapNonInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t readNonInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeMMapInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeMMapNonInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeInterleavedEnd(std::uint32_t frameSize, DataType& data);
    static snd_pcm_sframes_t writeNonInterleavedEnd(std::uint32_t frameSize, DataType& data);
    using ContainerType = std::vector<DataType>;
    enum TupleElementType
    {
        DeviceSelector,
        PCMHandle,
        ChannelCount,
        Format,
        Access,
        Buffer,
        NonInterleaveArray,
        MMapOffset,
        MMapFrames
    };
public:
    Impl();
    ~Impl();
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
    std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*, void**> activateDevice(bool isInput, ALSADeviceSelector selector);
    static std::shared_ptr<std::byte[]> allocateBuffer(std::uint32_t frameSize, std::uint32_t channelCount, snd_pcm_format_t format);
private:
    std::uint32_t sampleRate_;
    std::uint32_t frameSize_;
    ContainerType inputs_;
    ContainerType outputs_;
    std::thread audioThread_;
    std::atomic_flag runFlag_ {ATOMIC_FLAG_INIT};
    std::set<std::shared_ptr<std::byte[]>, Comparison<std::byte>> buffers_;
    std::set<std::shared_ptr<void*[]>, Comparison<void*>> nonInterleaveArrays_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_LINUX_ALSAAUDIOBACKENDIMPL
