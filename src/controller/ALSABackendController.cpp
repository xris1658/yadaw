#include "ALSABackendController.hpp"

#include "util/YAMLCppUtil.hpp"

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
YADAW::Audio::Backend::ALSABackend& appALSABackend()
{
    static YADAW::Audio::Backend::ALSABackend ret;
    return ret;
}

YADAW::Model::ALSAInputDeviceListModel& appALSAInputDeviceListModel()
{
    static YADAW::Model::ALSAInputDeviceListModel ret(appALSABackend());
    return ret;
}

YADAW::Model::ALSAOutputDeviceListModel& appALSAOutputDeviceListModel()
{
    static YADAW::Model::ALSAOutputDeviceListModel ret(appALSABackend());
    return ret;
}

void activateDefaultDevice(Audio::Backend::ALSABackend& backend)
{
    backend.setAudioInputDeviceActivated(
        YADAW::Audio::Backend::ALSADeviceSelector(0, 0),
        true
    );
    backend.setAudioOutputDeviceActivated(
        YADAW::Audio::Backend::ALSADeviceSelector(0, 0),
        true
    );
}

bool initializeALSAFromConfig(const YAML::Node& node)
{
    auto& backend = appALSABackend();
    const auto& sampleRateNode = node["sample-rate"];
    auto sampleRate = sampleRateNode.IsDefined()? sampleRateNode.as<std::uint32_t>(): DefaultSampleRate;
    const auto& frameSizeNode = node["buffer-size"];
    auto frameSize = frameSizeNode.IsDefined()? frameSizeNode.as<std::uint32_t>(): DefaultFrameSize;
    backend.initialize(sampleRate, frameSize);
    if(const auto& inputDevicesNode = node["inputs"];
        inputDevicesNode.IsDefined() && inputDevicesNode.IsSequence())
    {
        for(const auto& input: inputDevicesNode)
        {
            backend.setAudioInputDeviceActivated(
                YADAW::Audio::Backend::ALSADeviceSelector(
                    input["card-index"].as<int>(),
                    input["device-index"].as<int>()
                ),
                input["activated"].as<bool>()
            );
        }
    }
    if(const auto& outputDevicesNode = node["outputs"];
        outputDevicesNode.IsDefined() && outputDevicesNode.IsSequence())
    {
        for(const auto& output: outputDevicesNode)
        {
            backend.setAudioOutputDeviceActivated(
                YADAW::Audio::Backend::ALSADeviceSelector(
                    output["card-index"].as<int>(),
                    output["device-index"].as<int>()
                ),
                output["activated"].as<bool>()
            );
        }
    }
}

YADAW::Audio::Backend::ALSABusConfiguration& appAudioBusConfiguration()
{
    static YADAW::Audio::Backend::ALSABusConfiguration ret(appALSABackend());
    return ret;
}

YAML::Node deviceConfigFromALSA()
{
    using namespace YADAW::Util;
    YAML::Emitter emitter;
    {
        YAMLMap map(emitter);
        auto& backend = appALSABackend();
        emitter << YAML::Key << "inputs" << YAML::Value;
        {
            YAMLSeq seq(emitter);
            auto inputCount = backend.audioInputDeviceCount();
            for(decltype(inputCount) i = 0; i < inputCount; ++i)
            {
                YAMLMap map(emitter);
                const auto& device = backend.audioInputDeviceAt(i).value();
                emitter << YAML::Key << "card-index" << YAML::Value << device.cIndex;
                emitter << YAML::Key << "device-index" << YAML::Value << device.dIndex;
                emitter << YAML::Key << "activated" << YAML::Value << backend.isAudioInputDeviceActivated(device);
            }
        }
        emitter << YAML::Key << "outputs" << YAML::Value;
        {
            YAMLSeq seq(emitter);
            auto inputCount = backend.audioOutputDeviceCount();
            for(decltype(inputCount) i = 0; i < inputCount; ++i)
            {
                YAMLMap map(emitter);
                const auto& device = backend.audioOutputDeviceAt(i).value();
                emitter << YAML::Key << "card-index" << YAML::Value << device.cIndex;
                emitter << YAML::Key << "device-index" << YAML::Value << device.dIndex;
                emitter << YAML::Key << "activated" << YAML::Value << backend.isAudioOutputDeviceActivated(device);
            }
        }
        return YAML::Load(emitter.c_str());
    }
}
}