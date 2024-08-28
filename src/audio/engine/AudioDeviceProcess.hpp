#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS

#include "audio/device/IAudioDevice.hpp"
#include "util/Concepts.hpp"

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Device::AudioProcessData;

// A not-owning version of `std::function`
class AudioDeviceProcess
{
public:
    template<DerivedTo<YADAW::Audio::Device::IAudioDevice> Device>
    explicit AudioDeviceProcess(Device& device):
        audioDevice_(static_cast<YADAW::Audio::Device::IAudioDevice*>(&device)),
        func_(&doProcess<Device>)
    {}
    AudioDeviceProcess(const AudioDeviceProcess& rhs) = default;
    AudioDeviceProcess(AudioDeviceProcess&& rhs) = default;
    AudioDeviceProcess& operator=(const AudioDeviceProcess& rhs) = default;
    AudioDeviceProcess& operator=(AudioDeviceProcess&& rhs) = default;
    ~AudioDeviceProcess() = default;
private:
    template<DerivedTo<YADAW::Audio::Device::IAudioDevice> Device>
    static void doProcess(YADAW::Audio::Device::IAudioDevice* ptr,
        const AudioProcessData<float>& audioProcessData)
    {
        static_cast<Device*>(ptr)->process(audioProcessData);
    }
public:
    const YADAW::Audio::Device::IAudioDevice* device() const
    {
        return audioDevice_;
    }
    YADAW::Audio::Device::IAudioDevice* device()
    {
        return audioDevice_;
    }
    inline void process(const AudioProcessData<float>& audioProcessData)
    {
        func_(audioDevice_, audioProcessData);
    }
private:
    YADAW::Audio::Device::IAudioDevice* audioDevice_ = nullptr;
    void(*func_)(YADAW::Audio::Device::IAudioDevice*, const AudioProcessData<float>&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS
