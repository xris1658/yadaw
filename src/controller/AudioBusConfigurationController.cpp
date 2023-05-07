#include "AudioBusConfigurationController.hpp"

#include "audio/device/IAudioBusConfiguration.hpp"
#include "util/YAMLCppUtil.hpp"

namespace YADAW::Controller
{
void loadAudiousConfiguration(const YAML::Node& node,
    YADAW::Model::AudioBusConfigurationModel& model)
{
    if(node.IsSequence())
    {
        auto size = node.size();
        for(decltype(size) i = 0; i < size; ++i)
        {
            const auto& bus = node[i];
            const auto& name =
                QString::fromStdString(bus["name"].as<std::string>());
            if(const auto& channels = bus["channels"];
                channels.IsSequence())
            {
                auto channelCount = channels.size();
                model.append(channelCount);
                for(decltype(channelCount) j = 0; j < channelCount; ++j)
                {
                    const auto& channel = channels[j];
                    auto deviceIndex = channel["device-index"].as<int>();
                    auto channelIndex = channel["channel-index"].as<int>();
                    model.setChannel(i, j, deviceIndex, channelIndex);
                }
            }
        }
    }
}

void emitAudioChannelList(YAML::Emitter& emitter, const YADAW::Audio::Device::IBus& bus)
{
    using namespace YADAW::Util;
    emitter << YAML::Key << "channels";
    YAMLSeq seq(emitter);
    auto channelCount = bus.channelCount();
    for(decltype(channelCount) i = 0; i < channelCount; ++i)
    {
        const auto& channel = bus.channelAt(i).value();
        emitter << YAML::Key << "device-index" << YAML::Value << channel.deviceIndex;
        emitter << YAML::Key << "channel-index" << YAML::Value << channel.channelIndex;
    }
}

void loadAudioBusConfiguration(const YAML::Node& node,
    YADAW::Model::AudioBusConfigurationModel& inputModel,
    YADAW::Model::AudioBusConfigurationModel& outputModel)
{
    loadAudiousConfiguration(node, inputModel);
    loadAudiousConfiguration(node, outputModel);
}

YAML::Node exportFromAudioBusConfiguration(
    const YADAW::Audio::Device::IAudioBusConfiguration& configuration,
    const YADAW::Model::AudioBusConfigurationModel& inputModel,
    const YADAW::Model::AudioBusConfigurationModel& outputModel)
{
    using namespace YADAW::Util;
    YAML::Emitter emitter;
    YAMLMap map(emitter);
    emitter << YAML::Key << "input-buses" << YAML::Value;
    {
        YAMLSeq seq(emitter);
        auto inputCount = configuration.inputBusCount();
        for(decltype(inputCount) i = 0; i < inputCount; ++i)
        {
            const auto& bus = configuration.inputBusAt(i)->get();
            emitter << YAML::Key << "name" << YAML::Value
                << inputModel.nameAt(i)->get().toStdString();
            emitAudioChannelList(emitter, bus);
        }
    }
    emitter << YAML::Key << "output-buses" << YAML::Value;
    {
        YAMLSeq seq(emitter);
        auto outputCount = configuration.outputBusCount();
        for(decltype(outputCount) i = 0; i < outputCount; ++i)
        {
            const auto& bus = configuration.outputBusAt(i)->get();
            emitter << YAML::Key << "name" << YAML::Value
                << outputModel.nameAt(i)->get().toStdString();
            emitAudioChannelList(emitter, bus);
        }
    }
    return YAML::Load(emitter.c_str());
}
}