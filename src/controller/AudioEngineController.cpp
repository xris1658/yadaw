#include "AudioEngineController.hpp"

#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "util/Util.hpp"

#include <execution>

namespace YADAW::Controller
{
AudioEngine::AudioEngine():
    processSequence_(std::make_unique<YADAW::Audio::Engine::ProcessSequence>()),
    vst3PluginPool_(std::make_unique<YADAW::Controller::VST3PluginPoolVector>()),
    clapPluginPool_(std::make_unique<YADAW::Controller::CLAPPluginPoolVector>()),
    clapPluginToSetProcess_(std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>())
{
    mixer_.bufferExtension().setBufferSize(bufferSize_);
    mixer_.setNodeAddedCallback(&AudioEngine::mixerNodeAddedCallback);
    mixer_.setNodeRemovedCallback(&AudioEngine::mixerNodeRemovedCallback);
    mixer_.setConnectionUpdatedCallback(&AudioEngine::mixerConnectionUpdatedCallback);
}

AudioEngine& AudioEngine::appAudioEngine()
{
    static AudioEngine ret;
    return ret;
}

double AudioEngine::sampleRate() const
{
    return sampleRate_;
}

std::uint32_t AudioEngine::bufferSize() const
{
    return bufferSize_;
}

void AudioEngine::setSampleRate(double sampleRate)
{
    sampleRate_ = sampleRate;
    // FIXME: Reset all audio devices in the graph
}

void AudioEngine::setBufferSize(std::uint32_t bufferSize)
{
    bufferSize_ = bufferSize;
    mixer_.bufferExtension().setBufferSize(bufferSize_);
    // FIXME: Reset all audio devices in the graph
}

const YADAW::Audio::Mixer::Mixer& AudioEngine::mixer() const
{
    return mixer_;
}

YADAW::Audio::Mixer::Mixer& AudioEngine::mixer()
{
    return mixer_;
}

const YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Audio::Engine::ProcessSequence>>&
AudioEngine::processSequence() const
{
    return processSequence_;
}

YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Audio::Engine::ProcessSequence>>& AudioEngine::processSequence()
{
    return processSequence_;
}

void AudioEngine::uninitialize()
{
    mixer_.clearChannels();
    mixer_.clearAudioInputChannels();
    mixer_.clearAudioOutputChannels();
    processSequence_.update(
        std::make_unique<YADAW::Audio::Engine::ProcessSequence>()
    );
    processSequence_.swapIfNeeded();
    processSequence_.disposeOld();
    mixer_.graph().clearMultiInputNodes();
    mixer_.graph().graph().clear();
}

using SetCLAPPluginProcessing = decltype(&YADAW::Audio::Plugin::CLAPPlugin::startProcessing);

SetCLAPPluginProcessing setProcessing[2] = {
    &YADAW::Audio::Plugin::CLAPPlugin::stopProcessing,
    &YADAW::Audio::Plugin::CLAPPlugin::startProcessing
};

void AudioEngine::process()
{
    auto now = YADAW::Util::currentTimeValueInNanosecond();
    YADAW::Audio::Host::CLAPHost::setAudioThreadId(std::this_thread::get_id());
    processSequence_.swapIfNeeded();
    vst3PluginPool_.swapIfNeeded();
    clapPluginToSetProcess_.swapIfNeeded();
    clapPluginPool_.swapIfNeeded();
    YADAW::Controller::fillVST3InputParameterChanges(
        *(vst3PluginPool_.get()),
        YADAW::Util::currentTimeValueInNanosecond()
    );
    YADAW::Controller::fillCLAPInputParameterChanges(*(clapPluginPool_.get()),
        YADAW::Util::currentTimeValueInNanosecond());
    auto& clapPluginToSetProcess = *clapPluginToSetProcess_.get();
    for(const auto& [plugin, process]: clapPluginToSetProcess)
    {
        (plugin->*setProcessing[process])();
    }
    static_assert(std::is_trivially_destructible_v<YADAW::Controller::CLAPPluginToSetProcess>);
    // The assertion above passed.
    // Sadly, we do NOT know if `std::vector<T>::clear()` is O(1) if `T` is
    // trivially destructible on unknown STL implementations.
    // On MSVC STL, it is O(1). On libstdc++ and libcxx, it seems not.
    clapPluginToSetProcess.clear();
    auto& processSequence = *(processSequence_.get());
    std::for_each(processSequence.begin(), processSequence.end(),
        [](Vector2D<YADAW::Audio::Engine::ProcessPair>& row)
        {
        // TODO: Replace this with implementation based on thread pool
            std::for_each(std::execution::par_unseq, row.begin(), row.end(),
                [](std::vector<YADAW::Audio::Engine::ProcessPair>& cell)
                {
                    std::for_each(cell.begin(), cell.end(),
                        [](YADAW::Audio::Engine::ProcessPair& pair)
                        {
                            pair.first.process(pair.second.audioProcessData());
                        }
                    );
                }
            );
        }
    );
    processTime_.store(
        YADAW::Util::currentTimeValueInNanosecond() - now,
        std::memory_order_release
    );
}

void AudioEngine::updateProcessSequence()
{
    processSequence_.updateAndDispose(
        std::make_unique<YADAW::Audio::Engine::ProcessSequence>(
            YADAW::Audio::Engine::getProcessSequence(
                mixer_.graph().graph(),
                mixer_.bufferExtension()
            )
        ),
        running()
    );
}

void AudioEngine::mixerNodeAddedCallback(const Audio::Mixer::Mixer& mixer)
{
    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

void AudioEngine::mixerNodeRemovedCallback(const Audio::Mixer::Mixer& mixer)
{

    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

void AudioEngine::mixerConnectionUpdatedCallback(const Audio::Mixer::Mixer& mixer)
{

    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

void AudioEngine::insertsNodeAddedCallback(const Audio::Mixer::Inserts& inserts)
{

    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

void AudioEngine::insertsNodeRemovedCallback(const Audio::Mixer::Inserts& inserts)
{

    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

void AudioEngine::insertsConnectionUpdatedCallback(const Audio::Mixer::Inserts& inserts)
{

    auto& instance = AudioEngine::appAudioEngine();
    instance.updateProcessSequence();
}

const YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::VST3PluginPoolVector>>&
    AudioEngine::vst3PluginPool() const
{
    return vst3PluginPool_;
}

YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::VST3PluginPoolVector>>&
    AudioEngine::vst3PluginPool()
{
    return vst3PluginPool_;
}

const YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::CLAPPluginPoolVector>>&
AudioEngine::clapPluginPool() const
{
    return clapPluginPool_;
}

YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::CLAPPluginPoolVector>>&
AudioEngine::clapPluginPool()
{
    return clapPluginPool_;
}

const YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::CLAPPluginToSetProcessVector>>&
AudioEngine::clapPluginToSetProcess() const
{
    return clapPluginToSetProcess_;
}

YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<YADAW::Controller::CLAPPluginToSetProcessVector>>&
AudioEngine::clapPluginToSetProcess()
{
    return clapPluginToSetProcess_;
}

bool AudioEngine::running() const
{
    return running_;
}

void AudioEngine::setRunning(bool running)
{
    running_ = running;
}
}