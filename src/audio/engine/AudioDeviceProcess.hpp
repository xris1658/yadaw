#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEPROCESS

#include "audio/device/IAudioDevice.hpp"

#include <type_traits>

#define DECLVAL_FUNCTION(class_name, function_name, data_type) std::declval<class_name>().function_name(std::declval<data_type>())

namespace YADAW::Audio::Engine
{
using YADAW::Audio::Device::AudioProcessData;

// A not-owning version of `std::function`
class AudioDeviceProcess
{
private: // Expression SFINAE
    template<typename T, typename U = void>
    struct HasProcessMethodHelper: std::false_type {};
    template<typename T>
    struct HasProcessMethodHelper<T,
        std::void_t<decltype(DECLVAL_FUNCTION(T, process, const AudioProcessData<float>&))>
    >: std::true_type {};
    template<typename T>
    static constexpr bool hasProcess = HasProcessMethodHelper<T>::value;
public:
    template<typename T>
    explicit AudioDeviceProcess(T& audioDevice):
        audioDevice_(static_cast<YADAW::Audio::Device::IAudioDevice*>(&audioDevice)),
        func_(&doProcess<T>)
    {}
    AudioDeviceProcess(const AudioDeviceProcess& rhs) = default;
    AudioDeviceProcess(AudioDeviceProcess&& rhs) = default;
    AudioDeviceProcess& operator=(const AudioDeviceProcess& rhs) = default;
    AudioDeviceProcess& operator=(AudioDeviceProcess&& rhs) = default;
    ~AudioDeviceProcess() = default;
private:
    template<typename T>
    static void doProcess(YADAW::Audio::Device::IAudioDevice* ptr,
        const AudioProcessData<float>& audioProcessData)
    {
        static_assert(hasProcess<T>);
        static_cast<T*>(ptr)->process(audioProcessData);
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
