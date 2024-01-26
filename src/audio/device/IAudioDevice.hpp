#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
#define YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE

#include "audio/device/IAudioChannelGroup.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Audio::Device
{
template<typename SampleType>
struct AudioProcessData
{
    std::uint32_t inputGroupCount = 0;
    std::uint32_t outputGroupCount = 0;
    std::uint32_t singleBufferSize = 0;
    std::uint32_t* inputCounts = nullptr;
    std::uint32_t* outputCounts = nullptr;
    SampleType*** inputs = nullptr;
    SampleType*** outputs = nullptr;
};

class IAudioDevice
{
public:
    struct ChannelMap
    {
        ChannelMap(
            std::uint32_t inChannelGroup,
            std::uint32_t inChannel,
            std::uint32_t outChannelGroup,
            std::uint32_t outChannel):
            inChannelGroup(inChannelGroup),
            inChannel(inChannel),
            outChannelGroup(outChannelGroup),
            outChannel(outChannel)
        {}
        std::uint32_t inChannelGroup;
        std::uint32_t inChannel;
        std::uint32_t outChannelGroup;
        std::uint32_t outChannel;
    };
public:
    using OptionalAudioChannelGroup = OptionalRef<const IAudioChannelGroup>;
    using OptionalChannelMap = OptionalRef<const ChannelMap>;
public:
    virtual ~IAudioDevice() = default;
public:
    virtual std::uint32_t audioInputGroupCount() const = 0;
    virtual std::uint32_t audioOutputGroupCount() const = 0;
    virtual OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const = 0;
    virtual OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const = 0;
    virtual std::uint32_t latencyInSamples() const = 0;
    virtual std::uint32_t audioChannelMapCount() const = 0;
    virtual OptionalChannelMap audioChannelMapAt(std::uint32_t index) const = 0;
public:
    virtual void process(const AudioProcessData<float>& audioProcessData) = 0;
};
}

#endif // YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
