#include "AudioBackendController.hpp"

#include "native/AudioBackend.hpp"

#include <cstring>

namespace YADAW::Controller
{
namespace Impl
{
template<AudioBackend> const char* AudioBackendName = "";
template<> const char* AudioBackendName<AudioBackend::Dummy> = "Dummy";
template<> const char* AudioBackendName<AudioBackend::AudioGraph> = "AudioGraph";
template<> const char* AudioBackendName<AudioBackend::ALSA> = "ALSA";

template<AudioBackend> const char* AudioBackendNodeName = "";
template<> const char* AudioBackendNodeName<AudioBackend::Dummy> = "dummy";
template<> const char* AudioBackendNodeName<AudioBackend::AudioGraph> = "audiograph";
template<> const char* AudioBackendNodeName<AudioBackend::ALSA> = "alsa";

bool isAudioBackendSupported(AudioBackend backend)
{
    if(backend == AudioBackend::Dummy)
    {
        return YADAW::Native::isAudioBackendSupported<AudioBackend::Dummy>;
    }
    if(backend == AudioBackend::AudioGraph)
    {
        return YADAW::Native::isAudioBackendSupported<AudioBackend::AudioGraph>;
    }
    if(backend == AudioBackend::ALSA)
    {
        return YADAW::Native::isAudioBackendSupported<AudioBackend::ALSA>;
    }
    return false;
}
}
AudioBackend AudioBackendController::audioBackend() const
{
    return audioBackend_;
}

bool AudioBackendController::importConfiguration(const YAML::Node& node)
{
    try
    {
        auto backendString = node["audio-api"].as<std::string>();
        auto backend = AudioBackend::Invalid;
        if(std::strcmp(backendString.data(), Impl::AudioBackendName<AudioBackend::Dummy>) == 0
            && YADAW::Native::isAudioBackendSupported<AudioBackend::Dummy>)
        {
            const auto& dummyNode = node[Impl::AudioBackendNodeName<AudioBackend::Dummy>];
            // TODO
            return true;
        }
        else if(std::strcmp(backendString.data(), Impl::AudioBackendName<AudioBackend::AudioGraph>) == 0
            && YADAW::Native::isAudioBackendSupported<AudioBackend::AudioGraph>)
        {
            const auto& audioGraphNode = node[Impl::AudioBackendNodeName<AudioBackend::AudioGraph>];
            // TODO
            return true;
        }
        else if(std::strcmp(backendString.data(), Impl::AudioBackendName<AudioBackend::ALSA>) == 0
            && YADAW::Native::isAudioBackendSupported<AudioBackend::ALSA>)
        {
            const auto& alsaNode = node[Impl::AudioBackendNodeName<AudioBackend::ALSA>];
            // TODO
            return true;
        }
        else
        {
            return false;
        }
    }
    catch(...)
    {
        // TODO: generate and import default configuration and call it a day?
        return false;
    }
}

YAML::Node AudioBackendController::exportConfiguration() const
{
    // TODO
    return YAML::Node();
}
}
