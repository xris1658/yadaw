#ifndef YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER

#include "audio/device/IAudioDevice.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/mixer/Mixer.hpp"
#include "concurrent/PassDataToRealtimeThread.hpp"
#include "controller/CLAPPluginPool.hpp"
#include "controller/VST3PluginPool.hpp"

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
    const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::VST3PluginPoolVector>& vst3PluginPool() const;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::VST3PluginPoolVector>& vst3PluginPool();
    const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginPoolVector>& clapPluginPool() const;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginPoolVector>& clapPluginPool();
    const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginToSetProcessVector>& clapPluginToSetProcess() const;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginToSetProcessVector>& clapPluginToSetProcess();
    bool running() const;
    void setRunning(bool running);
public:
    void uninitialize();
    void process();
    std::int64_t processTime()
    {
        return processTime_.load(std::memory_order_acquire);
    }
private:
    void updateProcessSequence();
public:
    static void mixerNodeAddedCallback(const YADAW::Audio::Mixer::Mixer& mixer);
    static void mixerNodeRemovedCallback(const YADAW::Audio::Mixer::Mixer& mixer);
    static void mixerConnectionUpdatedCallback(const YADAW::Audio::Mixer::Mixer& mixer);
    static void insertsNodeAddedCallback(const YADAW::Audio::Mixer::Inserts& inserts);
    static void insertsNodeRemovedCallback(const YADAW::Audio::Mixer::Inserts& inserts);
    static void insertsConnectionUpdatedCallback(const YADAW::Audio::Mixer::Inserts& inserts);
private:
    double sampleRate_ = 0.0;
    std::uint32_t bufferSize_ = 0U;
    bool running_ = false;
    std::atomic<std::int64_t> processTime_;
    YADAW::Audio::Mixer::Mixer mixer_;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence> processSequence_;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::VST3PluginPoolVector> vst3PluginPool_;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginPoolVector> clapPluginPool_;
    YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::CLAPPluginToSetProcessVector> clapPluginToSetProcess_;
};
}

#endif // YADAW_SRC_CONTROLLER_AUDIOENGINECONTROLLER
