#if __linux__

#include "ALSABackendController.hpp"

#include "controller/AudioEngineController.hpp"
#include "util/IntegerRange.hpp"
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

std::vector<QString> activateDefaultDevice(YADAW::Audio::Backend::ALSABackend& backend)
{
    std::vector<QString> ret;
    auto defaultDeviceSelector = YADAW::Audio::Backend::ALSADeviceSelector::defaultDeviceSelector();
    auto inputIndex = YADAW::Audio::Backend::findDeviceBySelector(true, defaultDeviceSelector);
    if(inputIndex.has_value())
    {
        auto result = backend.setAudioDeviceActivated(true, *inputIndex, true);
        if(result.second != 0)
        {
            ret.emplace_back(
                YADAW::Audio::Backend::getALSAErrorString(
                    true, defaultDeviceSelector, result
                )
            );
        }
    }
    else
    {
        ret.emplace_back(
            YADAW::Audio::Backend::getALSAErrorString(
                true, defaultDeviceSelector, {}
            )
        );
    }
    auto outputIndex = YADAW::Audio::Backend::findDeviceBySelector(false, defaultDeviceSelector);
    if(outputIndex.has_value())
    {
        auto result = backend.setAudioDeviceActivated(false, *outputIndex, true);
        if(result.second != 0)
        {
            ret.emplace_back(
                YADAW::Audio::Backend::getALSAErrorString(
                    false, defaultDeviceSelector, result
                )
            );
        }
    }
    else
    {
        ret.emplace_back(
            YADAW::Audio::Backend::getALSAErrorString(
                false, defaultDeviceSelector, {}
            )
        );
    }
    return ret;
}

std::vector<QString> initializeALSAFromConfig(const YAML::Node& node)
{
    std::vector<QString> ret;
    auto& backend = appALSABackend();
    const auto& sampleRateNode = node["sample-rate"];
    auto sampleRate = sampleRateNode.as<std::uint32_t>(DefaultSampleRate);
    const auto& frameSizeNode = node["buffer-size"];
    auto frameSize = frameSizeNode.as<std::uint32_t>(DefaultFrameSize);
    backend.initialize(sampleRate, frameSize);
    const auto& inputDevicesNode = node["inputs"];
    const auto& outputDevicesNode = node["outputs"];
    if(inputDevicesNode.IsDefined() || outputDevicesNode.IsDefined())
    {
        if(inputDevicesNode.IsSequence())
        {
            for(const auto& input: inputDevicesNode)
            {
                auto selector = YADAW::Audio::Backend::ALSADeviceSelector(
                    input["card-index"].as<std::uint32_t>(YADAW::Audio::Backend::InvalidIndex),
                    input["device-index"].as<std::uint32_t>(YADAW::Audio::Backend::InvalidIndex)
                );
                auto index = YADAW::Audio::Backend::findDeviceBySelector(
                    true, selector
                );
                if(index.has_value())
                {
                    auto result = backend.setAudioDeviceActivated(
                        true, *index, input["activated"].as<bool>(false)
                    );
                    if(result.second != 0)
                    {
                        ret.emplace_back(
                            YADAW::Audio::Backend::getALSAErrorString(
                                true, selector, result
                            )
                        );
                    }
                }
                else
                {
                    ret.emplace_back(
                        YADAW::Audio::Backend::getALSAErrorString(
                            true, selector, {}
                        )
                    );
                }
            }
        }
        if(outputDevicesNode.IsSequence())
        {
            for(const auto& output: outputDevicesNode)
            {
                auto selector = YADAW::Audio::Backend::ALSADeviceSelector(
                    output["card-index"].as<std::uint32_t>(YADAW::Audio::Backend::InvalidIndex),
                    output["device-index"].as<std::uint32_t>(YADAW::Audio::Backend::InvalidIndex)
                );
                auto index = YADAW::Audio::Backend::findDeviceBySelector(
                    false, selector
                );
                if(index.has_value())
                {
                    auto result = backend.setAudioDeviceActivated(
                        false, *index, output["activated"].as<bool>(false)
                    );
                    if(result.second != 0)
                    {
                        ret.emplace_back(
                            YADAW::Audio::Backend::getALSAErrorString(
                                false, selector, result
                            )
                        );
                    }
                }
                else
                {
                    ret.emplace_back(
                        YADAW::Audio::Backend::getALSAErrorString(
                            false, selector, {}
                        )
                    );
                }
            }
        }
    }
    else
    {
        ret = activateDefaultDevice(backend);
    }
    return ret;
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
            FOR_RANGE0(i, inputCount)
            {
                YAMLMap map(emitter);
                const auto& device = backend.audioInputDeviceAt(i).value();
                emitter << YAML::Key << "card-index" << YAML::Value << device.cIndex;
                emitter << YAML::Key << "device-index" << YAML::Value << device.dIndex;
                emitter << YAML::Key << "activated" << YAML::Value << backend.isAudioDeviceActivated(true, i);
            }
        }
        emitter << YAML::Key << "outputs" << YAML::Value;
        {
            YAMLSeq seq(emitter);
            auto outputCount = backend.audioOutputDeviceCount();
            FOR_RANGE0(i, outputCount)
            {
                YAMLMap map(emitter);
                const auto& device = backend.audioOutputDeviceAt(i).value();
                emitter << YAML::Key << "card-index" << YAML::Value << device.cIndex;
                emitter << YAML::Key << "device-index" << YAML::Value << device.dIndex;
                emitter << YAML::Key << "activated" << YAML::Value << backend.isAudioDeviceActivated(false, i);
            }
        }
        return YAML::Load(emitter.c_str());
    }
}

void alsaCallback(const YADAW::Audio::Backend::ALSABackend* backend,
    std::uint32_t inputCount, YADAW::Audio::Backend::ALSABackend::AudioBuffer* const inputBuffers,
    std::uint32_t outputCount, YADAW::Audio::Backend::ALSABackend::AudioBuffer* const outputBuffers)
{
    appAudioBusConfiguration().setBuffers(inputBuffers, outputBuffers);
    YADAW::Controller::AudioEngine::appAudioEngine().process();
}
}

#endif