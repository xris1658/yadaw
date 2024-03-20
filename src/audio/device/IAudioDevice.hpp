#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
#define YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE

#include "audio/device/IAudioChannelGroup.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Audio::Device
{
template<typename SampleType>
struct AudioProcessData
{
private:
    using Self = AudioProcessData<SampleType>;
public:
    std::uint32_t inputGroupCount = 0;
    std::uint32_t outputGroupCount = 0;
    std::uint32_t singleBufferSize = 0;
    std::uint32_t* inputCounts = nullptr;
    std::uint32_t* outputCounts = nullptr;
    SampleType*** inputs = nullptr;
    SampleType*** outputs = nullptr;
    AudioProcessData() = default;
    AudioProcessData(const Self& rhs) = default;
    AudioProcessData(Self&& rhs) noexcept:
        inputGroupCount(rhs.inputGroupCount),
        outputGroupCount(rhs.outputGroupCount),
        singleBufferSize(rhs.singleBufferSize),
        inputCounts(rhs.inputCounts),
        outputCounts(rhs.outputCounts),
        inputs(rhs.inputs),
        outputs(rhs.outputs)
    {
        std::memset(&rhs, 0, sizeof(Self));
    }
    Self& operator=(const Self& rhs)
    {
        if(this != &rhs)
        {
            inputGroupCount = rhs.inputGroupCount;
            outputGroupCount = rhs.outputGroupCount;
            singleBufferSize = rhs.singleBufferSize;
            inputCounts = rhs.inputCounts;
            outputCounts = rhs.outputCounts;
            inputs = rhs.inputs;
            outputs = rhs.outputs;
        }
        return *this;
    }
    Self& operator=(Self&& rhs) noexcept
    {
        if(this != &rhs)
        {
            inputGroupCount = rhs.inputGroupCount;
            outputGroupCount = rhs.outputGroupCount;
            singleBufferSize = rhs.singleBufferSize;
            inputCounts = rhs.inputCounts;
            outputCounts = rhs.outputCounts;
            inputs = rhs.inputs;
            outputs = rhs.outputs;
            std::memset(&rhs, 0, sizeof(Self));
        }
        return *this;
    }
    ~AudioProcessData() noexcept = default;
};

class IAudioDevice
{
public:
    using OptionalAudioChannelGroup = OptionalRef<const IAudioChannelGroup>;
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
