#if _WIN32

#include "AudioGraphBackendController.hpp"

#include "controller/AudioEngineController.hpp"
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

YADAW::Native::ErrorCodeType activateDefaultDevice(
    YADAW::Audio::Backend::AudioGraphBackend& backend)
{
    YADAW::Native::ErrorCodeType ret = ERROR_SUCCESS;
    auto outputIndex = backend.defaultAudioOutputDeviceIndex();
    if(outputIndex != -1)
    {
        ret = backend.createAudioGraph(
            backend.audioOutputDeviceAt(outputIndex).id
        );
        if(ret == ERROR_SUCCESS)
        {
            auto inputIndex = backend.defaultAudioInputDeviceIndex();
            if(inputIndex != -1)
            {
                ret = backend.activateDeviceInput(inputIndex, true);
            }
        }
    }
    return ret;
}

YADAW::Native::ErrorCodeType createAudioGraphFromConfig(const YAML::Node& node)
{
    YADAW::Native::ErrorCodeType ret = ERROR_SUCCESS;
    auto& backend = appAudioGraphBackend();
    backend.initialize();
    if(const auto& defaultOutputIdNode = node["default-output-id"];
        !defaultOutputIdNode.IsDefined())
    {
        if(const auto index = backend.defaultAudioOutputDeviceIndex();
            index == -1)
        {
            ret = backend.createAudioGraph();
        }
        else
        {
            ret = backend.createAudioGraph(backend.audioOutputDeviceAt(index).id);
        }
    }
    else
    {
        ret = backend.createAudioGraph(
            QString::fromStdString(
                defaultOutputIdNode.as<std::string>(std::string{})
            )
        );
    }
    if(ret != ERROR_SUCCESS)
    {
        return ret;
    }
    const auto& inputsNode = node["inputs"];
    if(inputsNode.IsDefined() && inputsNode.IsSequence())
    {
        auto configInputCount = inputsNode.size();
        auto deviceInputCount = backend.audioInputDeviceCount();
        FOR_RANGE0(i, configInputCount)
        {
            const auto& id = inputsNode[i]["id"].as<std::string>(std::string{});
            if(inputsNode[i]["activated"].as<bool>(false))
            {
                for(decltype(deviceInputCount) j = 0; j < deviceInputCount; ++j)
                {
                    if(const auto& device = backend.audioInputDeviceAt(j);
                        device.id == id.c_str())
                    {
                        auto code = backend.activateDeviceInput(j, true);
                        if(code != ERROR_SUCCESS)
                        {
                            ret = code;
                        }
                    }
                }
            }
        }
    }
    return ret;
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
            FOR_RANGE0(i, inputCount)
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

void audioGraphCallback(
    int inputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    appAudioBusConfiguration().setBuffers(inputs, outputs);
    YADAW::Controller::AudioEngine::appAudioEngine().process();
}
}

#endif