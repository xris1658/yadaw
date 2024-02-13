#ifndef YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER

#include "audio/device/IAudioDevice.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/mixer/Mixer.hpp"

#include "concurrent/PassDataToRealtimeThread.hpp"

namespace YADAW::Controller
{
namespace Impl
{
template<typename T>
void process(YADAW::Audio::Device::IAudioDevice* device,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    static_assert(std::is_base_of_v<YADAW::Audio::Device::IAudioDevice, T>
        && (!std::is_same_v<YADAW::Audio::Device::IAudioDevice, T>));
    static_cast<T*>(device)->process(audioProcessData);
}
}

class AudioEngine
{
private:
    AudioEngine();
public:
    static AudioEngine& appAudioEngine();
public:
    double sampleRate() const;
    std::uint32_t bufferSize() const;
    void setSampleRate(double sampleRate);
    void setBufferSize(std::uint32_t bufferSize);
    const YADAW::Audio::Mixer::Mixer& mixer() const;
    YADAW::Audio::Mixer::Mixer& mixer();
    const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence>& processSequence() const;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence>& processSequence();
public:
    void uninitialize();
    void process();
private:
    double sampleRate_ = 0.0;
    std::uint32_t bufferSize_ = 0U;
    YADAW::Audio::Mixer::Mixer mixer_;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence> processSequence_;
};
}

#endif // YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
