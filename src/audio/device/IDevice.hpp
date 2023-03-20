#ifndef YADAW_SRC_AUDIO_DEVICE_IDEVICE
#define YADAW_SRC_AUDIO_DEVICE_IDEVICE

#include "audio/device/IChannelGroup.hpp"

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

class IDevice
{
public:
    virtual ~IDevice() = default;
public:
    virtual int audioInputGroupCount() const = 0;
    virtual int audioOutputGroupCount() const = 0;
    virtual const YADAW::Audio::Device::IChannelGroup& audioInputGroupAt(int index) const = 0;
    virtual const YADAW::Audio::Device::IChannelGroup& audioOutputGroupAt(int index) const = 0;
    virtual std::uint32_t latencyInSamples() const = 0;
public:
    virtual void process(const AudioProcessData<float>& audioProcessData) = 0;
};
}

#endif //YADAW_SRC_AUDIO_DEVICE_IDEVICE
