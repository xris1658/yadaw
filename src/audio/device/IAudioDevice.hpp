#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
#define YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE

#include "audio/device/IAudioChannelGroup.hpp"

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
    virtual ~IAudioDevice() = default;
public:
    virtual int audioInputGroupCount() const = 0;
    virtual int audioOutputGroupCount() const = 0;
    virtual const IAudioChannelGroup* audioInputGroupAt(int index) const = 0;
    virtual const IAudioChannelGroup* audioOutputGroupAt(int index) const = 0;
    virtual std::uint32_t latencyInSamples() const = 0;
public:
    virtual void process(const AudioProcessData<float>& audioProcessData) = 0;
};
}

#endif //YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
