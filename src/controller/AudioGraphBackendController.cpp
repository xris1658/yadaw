#if(WIN32)

#include "AudioGraphBackendController.hpp"


#include "util/YAMLCppUtil.hpp"
#include "util/IntegerRange.hpp"

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

bool createAudioGraphFromConfig(const YAML::Node& node)
{
    auto& backend = appAudioGraphBackend();
    backend.initialize();
    if(const auto& defaultOutputIdNode = node["default-output-id"];
        !defaultOutputIdNode.IsDefined())
    {
        if(const auto index = backend.defaultAudioOutputDeviceIndex();
            index == -1)
        {
            if(!backend.createAudioGraph())
            {
                return false;
            }
        }
        else
        {
            if(!backend.createAudioGraph(backend.audioOutputDeviceAt(index).id))
            {
                return false;
            }
        }
    }
    else
    {
        if(!backend.createAudioGraph(
            QString::fromStdString(defaultOutputIdNode.as<std::string>())))
        {
            return false;
        }
    }
    const auto& inputsNode = node["inputs"];
    if(inputsNode.IsDefined() && inputsNode.IsSequence())
    {
        auto configInputCount = inputsNode.size();
        auto deviceInputCount = backend.audioInputDeviceCount();
        for(auto i: YADAW::Util::IntegerRange(configInputCount))
        {
            const auto& id = inputsNode[i]["id"].as<std::string>();
            if(inputsNode[i]["activated"].as<bool>())
            {
                for(decltype(deviceInputCount) j = 0; j < deviceInputCount; ++j)
                {
                    if(const auto& device = backend.audioInputDeviceAt(j);
                        device.id == id.c_str())
                    {
                        backend.activateDeviceInput(j, true);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

YAML::Node deviceConfigFromCurrentAudioGraph()
{
    using namespace YADAW::Util;
    YAML::Emitter emitter;
    {
        YAMLMap map(emitter);
        auto& backend = appAudioGraphBackend();
        emitter << YAML::Key << "inputs" << YAML::Value;
        {
            YAMLSeq seq(emitter);
            auto inputCount = appAudioGraphBackend().audioInputDeviceCount();
            for(auto i: YADAW::Util::IntegerRange(inputCount))
            {
                YAMLMap map(emitter);
                const auto& device = backend.audioInputDeviceAt(i);
                auto activated = backend.isDeviceInputActivated(i);
                emitter << YAML::Key << "id" << YAML::Value << device.id.toStdString();
                emitter << YAML::Key << "activated" << YAML::Value << activated;
            }
        }
        emitter << YAML::Key << "default-output-id" << YAML::Value << backend.currentOutputDevice().id.toStdString();
    }
    return YAML::Load(emitter.c_str());
}

YADAW::Audio::Backend::AudioGraphBusConfiguration& appAudioBusConfiguration()
{
    static YADAW::Audio::Backend::AudioGraphBusConfiguration ret(appAudioGraphBackend());
    return ret;
}
}

#endif