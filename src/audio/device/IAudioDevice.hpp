#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
#define YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE

#include "audio/device/IAudioChannelGroup.hpp"

#include <functional>
#include <optional>

namespace YADAW::Audio::Device
{
template<typename SampleType>
struct AudioProcessData
{
    std::uint16_t inputGroupCount = 0;
    std::uint16_t outputGroupCount = 0;
    std::uint32_t singleBufferSize = 0;
    std::uint32_t* inputCounts = nullptr;
    std::uint32_t* outputCounts = nullptr;
    SampleType*** inputs = nullptr;
    SampleType*** outputs = nullptr;
};

class IAudioDevice
{
public:
    using OptionalAudioChannelGroup = std::optional<std::reference_wrapper<const IAudioChannelGroup>>;
public:
    virtual ~IAudioDevice() = default;
public:
    virtual std::uint32_t audioInputGroupCount() const = 0;
    virtual std::uint32_t audioOutputGroupCount() const = 0;
    virtual OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const = 0;
    virtual OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const = 0;
    virtual std::uint32_t latencyInSamples() const = 0;
public:
    virtual void process(const AudioProcessData<float>& audioProcessData) = 0;
};
}

#endif // YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
