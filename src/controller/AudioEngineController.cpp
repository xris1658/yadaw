#include "AudioEngineController.hpp"

#include "audio/host/CLAPHost.hpp"
#include "audio/host/HostContext.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "native/CPU.hpp"
#include "util/Util.hpp"

namespace YADAW::Controller
{
YADAW::Native::CPUTopology& getCPUTopology()
{
    static YADAW::Native::CPUTopology ret = YADAW::Native::getCPUTopology();
    return ret;
}

AudioEngine::AudioEngine():
    mixer_(
        [this](
            YADAW::Audio::Base::ChannelGroupType channelGroupType,
            std::uint32_t channelCountInGroup = 0)
        {
            return createVolumeFader(channelGroupType, channelCountInGroup);
        },
        [this](
            YADAW::Audio::Base::ChannelGroupType channelGroupType,
            std::uint32_t channelCountInGroup = 0)
        {
            return createMeter(channelGroupType, channelCountInGroup);
        }
    ),
    workerThreadPool_(
        std::make_unique<YADAW::Audio::Engine::ProcessSequenceWithPrev>(
            YADAW::Audio::Engine::getProcessSequenceWithPrev(
                mixer_.graph().graph(),
                mixer_.bufferExtension()
            )
        )
    ),
    vst3PluginPool_(std::make_unique<YADAW::Controller::VST3PluginPoolVector>()),
    clapPluginPool_(std::make_unique<YADAW::Controller::CLAPPluginPoolVector>()),
    clapPluginToSetProcess_(std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>())
{
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

void AudioEngine::initialize(double sampleRate, std::uint32_t bufferSize)
{
    sampleRate_ = sampleRate;
    bufferSize_ = bufferSize;
    mixer_.bufferExtension().setBufferSize(bufferSize_);
    // FIXME: Reset all audio devices in the graph
    FOR_RANGE0(i, mixer_.audioInputChannelCount())
    {
        mixer_.audioInputVolumeFaderAt(i)->get().initialize(sampleRate, bufferSize);
    }
    FOR_RANGE0(i, mixer_.audioOutputChannelCount())
    {
        mixer_.audioOutputVolumeFaderAt(i)->get().initialize(sampleRate, bufferSize);
    }
    FOR_RANGE0(i, mixer_.channelCount())
    {
        mixer_.volumeFaderAt(i)->get().initialize(sampleRate, bufferSize);
    }
}

const YADAW::Audio::Mixer::Mixer& AudioEngine::mixer() const
{
    return mixer_;
}

YADAW::Audio::Mixer::Mixer& AudioEngine::mixer()
{
    return mixer_;
}

const YADAW::Audio::Engine::AudioEngineWorkerThreadPool& AudioEngine::workerThreadPool() const
{
    return workerThreadPool_;
}

YADAW::Audio::Engine::AudioEngineWorkerThreadPool& AudioEngine::workerThreadPool()
{
    return workerThreadPool_;
}

void AudioEngine::uninitialize()
{
    mixer_.clearChannels();
    mixer_.clearAudioInputChannels();
    mixer_.clearAudioOutputChannels();
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
    YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.flip();
    YADAW::Audio::Host::CLAPHost::setAudioThreadId(std::this_thread::get_id());
    vst3PluginPool_.swapIfNeeded();
    clapPluginToSetProcess_.swapAndUseIfNeeded(
        [this](decltype(clapPluginToSetProcess_.get())& ptr)
        {
            for(const auto& [plugin, process]: *ptr)
            {
                (plugin->*setProcessing[process])();
            }
            static_assert(
                std::is_trivially_destructible_v<
                    YADAW::Controller::CLAPPluginToSetProcess
                >
            );
            // The assertion above passed.
            // Sadly, we do NOT know if `std::vector<T>::clear()` is O(1) even
            // if `T` is trivially destructible on unknown STL implementations.
            // On MSVC STL, it is O(1). On libstdc++ and libcxx, it seems not.
            ptr->clear();
        }
    );
    clapPluginPool_.swapIfNeeded();
    YADAW::Controller::fillVST3InputParameterChanges(
        *(vst3PluginPool_.get()),
        YADAW::Util::currentTimeValueInNanosecond()
    );
    YADAW::Controller::fillCLAPInputParameterChanges(*(clapPluginPool_.get()),
        YADAW::Util::currentTimeValueInNanosecond());
    FOR_RANGE0(i, mixer_.audioInputChannelCount())
    {
        mixer_.audioInputVolumeFaderAt(i)->get().onBufferSwitched(now);
    }
    FOR_RANGE0(i, mixer_.channelCount())
    {
        mixer_.volumeFaderAt(i)->get().onBufferSwitched(now);
    }
    FOR_RANGE0(i, mixer_.audioOutputChannelCount())
    {
        mixer_.audioOutputVolumeFaderAt(i)->get().onBufferSwitched(now);
    }
    workerThreadPool_.mainFunc();
    processTime_.store(
        YADAW::Util::currentTimeValueInNanosecond() - now,
        std::memory_order_release
    );
}

void AudioEngine::updateProcessSequence()
{
    workerThreadPool_.updateProcessSequence(
        std::make_unique<YADAW::Audio::Engine::ProcessSequenceWithPrev>(
            YADAW::Audio::Engine::getProcessSequenceWithPrev(
                mixer_.graph().graph(), mixer_.bufferExtension()
            )
        )
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
    if((!running_) && running)
    {
        const auto& cores = getCPUTopology()[0][0];
        std::vector<std::uint16_t> affinities;
        Vec<Vec<std::uint16_t>> coresVec;
        std::uint16_t logicalProcessorCount = 0;
        for(const auto& [core, logicalProcessors]: cores)
        {
            logicalProcessorCount += logicalProcessors.size();
            auto& processors = coresVec.emplace_back();
            processors.reserve(logicalProcessors.size());
            std::copy_n(logicalProcessors.rbegin(), logicalProcessors.size(), std::back_inserter(processors));
        }
        auto efficiencyCoreBegin = std::stable_partition(
            coresVec.begin(), coresVec.end(),
            [](Vec<std::uint16_t>& vec)
            {
                return vec.size() > 1;
            }
        );
        auto performanceCoreProcessorCount = 0;
        for(auto it = coresVec.begin(); it != efficiencyCoreBegin; ++it)
        {
            performanceCoreProcessorCount += it->size();
        }
        auto it = coresVec.begin();
        FOR_RANGE0(i, performanceCoreProcessorCount)
        {
            while(it->empty())
            {
                if(++it == efficiencyCoreBegin)
                {
                    it = coresVec.begin();
                }
            }
            affinities.emplace_back(it->back());
            it->pop_back();
            if(++it == efficiencyCoreBegin)
            {
                it = coresVec.begin();
            }
        }
        for(auto it = efficiencyCoreBegin; it != coresVec.end(); ++it)
        {
            affinities.emplace_back(it->front());
        }
        workerThreadPool_.setAffinities(affinities);
        workerThreadPool_.start();
    }
    else if(running_ && (!running))
    {
        workerThreadPool_.stop();
    }
    running_ = running;
}

std::unique_ptr<YADAW::Audio::Mixer::VolumeFader> AudioEngine::createVolumeFader(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup
)
{
    auto ret = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(channelGroupType, channelCountInGroup);
    ret->initialize(sampleRate_, bufferSize_);
    return ret;
}

std::unique_ptr<YADAW::Audio::Mixer::Meter> AudioEngine::createMeter(
    YADAW::Audio::Base::ChannelGroupType channelGroupType, std::uint32_t channelCountInGroup)
{
    return std::make_unique<YADAW::Audio::Mixer::Meter>(
        8192, channelGroupType, channelCountInGroup
    );
}
}
