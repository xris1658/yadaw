#include "AudioBackendController.hpp"

#include <yaml-cpp/emitter.h>

namespace YADAW::Controller
{
YADAW::Audio::Backend::AudioGraphBackend& appAudioGraphBackend()
{
    static YADAW::Audio::Backend::AudioGraphBackend ret;
    return ret;
}

YADAW::Model::AudioGraphInputDeviceListModel& appAudioGraphInputDeviceListModel()
{
    static YADAW::Model::AudioGraphInputDeviceListModel ret(appAudioGraphBackend());
    return ret;
}

YADAW::Model::AudioGraphOutputDeviceListModel& appAudioGraphOutputDeviceListModel()
{
    static YADAW::Model::AudioGraphOutputDeviceListModel ret(appAudioGraphBackend());
    return ret;
}

void activateDefaultDevice(YADAW::Audio::Backend::AudioGraphBackend& backend)
{
    auto outputIndex = backend.defaultAudioOutputDeviceIndex();
    if(outputIndex != -1)
    {
        backend.createAudioGraph(backend.audioOutputDeviceAt(outputIndex).id);
    }
    auto inputIndex = backend.defaultAudioInputDeviceIndex();
    if(inputIndex != -1)
    {
        backend.activateDeviceInput(inputIndex, true);
    }
}

YAML::Node deviceStateFromCurrentAudioBackend()
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "inputs" << YAML::Value << YAML::BeginSeq;
    auto& backend = appAudioGraphBackend();
    auto inputCount = appAudioGraphBackend().audioInputDeviceCount();
    for(decltype(inputCount) i = 0; i < inputCount; ++i)
    {
        emitter << YAML::BeginMap;
        const auto& device = backend.audioInputDeviceAt(i);
        auto activated = backend.isDeviceInputActivated(i);
        emitter << YAML::Key << "id" << YAML::Value << device.id.toStdString();
        emitter << YAML::Key << "activated" << YAML::Value << activated;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq << YAML::Key << "default-output-id" << YAML::Value << backend.audioOutputDeviceAt(backend.defaultAudioOutputDeviceIndex()).id.toStdString();
    emitter << YAML::EndMap;
    const auto* str = emitter.c_str();
    return YAML::Load(emitter.c_str());
}
}