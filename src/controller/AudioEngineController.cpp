#include "AudioEngineController.hpp"
#include "util/Util.hpp"

#include <execution>

namespace YADAW::Controller
{
AudioEngine::AudioEngine():
    processSequence_(std::make_unique<YADAW::Audio::Engine::ProcessSequence>()),
    vst3PluginPool_(std::make_unique<YADAW::Controller::VST3PluginPoolVector>())
{
    mixer_.bufferExtension().setBufferSize(bufferSize_);
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

const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence>&
AudioEngine::processSequence() const
{
    return processSequence_;
}

YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Audio::Engine::ProcessSequence>& AudioEngine::processSequence()
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

void AudioEngine::process()
{
    processSequence_.swapIfNeeded();
    vst3PluginPool_.swapIfNeeded();
    YADAW::Controller::fillVST3InputParameterChanges(
        *(vst3PluginPool_.get()),
        YADAW::Util::currentTimeValueInNanosecond()
    );
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
}

const YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::VST3PluginPoolVector>&
    AudioEngine::vst3PluginPool() const
{
    return vst3PluginPool_;
}

YADAW::Concurrent::PassDataToRealtimeThread<YADAW::Controller::VST3PluginPoolVector>&
    AudioEngine::vst3PluginPool()
{
    return vst3PluginPool_;
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