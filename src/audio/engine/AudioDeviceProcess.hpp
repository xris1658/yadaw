#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS

#include "audio/device/IAudioDevice.hpp"

#include <type_traits>

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Device::IAudioDevice;
using YADAW::Audio::Device::AudioProcessData;

// Why use virtual functions while you can do this?
// Implementing traits with expression SFINAE might be better.
class AudioDeviceProcess
{
public:
    template<typename T>
    explicit AudioDeviceProcess(T& audioDevice):
        audioDevice_(static_cast<IAudioDevice*>(&audioDevice)),
        func_(&doProcess<T>)
    {
    }
private:
    template<typename T>
    static void doProcess(IAudioDevice* audioDevice,
        const AudioProcessData<float>& audioProcessData)
    {
        static_assert(std::is_base_of_v<IAudioDevice, T>
                      && (!std::is_same_v<IAudioDevice, T>),
            "Error: T must be derived from IAudioDevice");
        static_cast<T*>(audioDevice)->process(audioProcessData);
    }
public:
    void process(const AudioProcessData<float>& audioProcessData)
    {
        func_(audioDevice_, audioProcessData);
    }
private:
    IAudioDevice* audioDevice_;
    void(*func_)(IAudioDevice*, const AudioProcessData<float>&);
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS
