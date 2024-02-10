#include "AudioEngineController.hpp"

namespace YADAW::Controller
{
AudioEngine::AudioEngine()
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

void AudioEngine::uninitialize()
{
    mixer_.clearChannels();
    mixer_.clearAudioInputChannels();
    mixer_.clearAudioOutputChannels();
    auto& graphWithPDC = mixer_.graph();
    auto& graph = graphWithPDC.graph();
    graphWithPDC.clearMultiInputNodes();
    graph.clear();
}
}