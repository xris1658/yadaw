#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE
#define YADAW_SRC_AUDIO_DEVICE_IAUDIODEVICE

#include "audio/device/IAudioChannelGroup.hpp"

namespace YADAW::Audio::Device
{
template<typename SampleType>
struct AudioProcessData
{
    int inputGroupCount = 0;
    int outputGroupCount = 0;
    int singleBufferSize = 0;
    int* inputCounts = nullptr;
    int* outputCounts = nullptr;
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
