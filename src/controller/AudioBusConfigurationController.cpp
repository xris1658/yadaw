#include "AudioBusConfigurationController.hpp"

#include "audio/device/IAudioBusConfiguration.hpp"
#include "entity/ChannelConfigHelper.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"
#include "util/YAMLCppUtil.hpp"

#include <iterator>
#include <string>

namespace YADAW::Controller
{
constexpr const char* channelConfigNames[] = {
    "invalid",
    "custom",
    "empty",
    "mono",
    "stereo",
    "lrc",
    "quad",
    "5.0",
    "5.1",
    "6.1",
    "7.1"
};

void loadAudioBusConfiguration(const YAML::Node& node,
    YADAW::Model::AudioBusConfigurationModel& model)
{
    if(node.IsSequence())
    {
        auto size = node.size();
        FOR_RANGE0(i, size)
        {
            const auto& bus = node[i];
            const auto& name = QString::fromStdString(
                bus["name"].as<std::string>(std::string{}));
            const auto& channelConfigName =
                bus["channel-config"].as<std::string>(std::string{});
            auto channelGroupType = YADAW::Audio::Base::ChannelGroupType::eInvalid;
            FOR_RANGE0(i, std::size(channelConfigNames))
            {
                if(std::strcmp(channelConfigNames[i], channelConfigName.c_str()) == 0)
                {
                    channelGroupType = static_cast<YADAW::Audio::Base::ChannelGroupType>(i - 2);
                    break;
                }
            }
            if(channelGroupType != YADAW::Audio::Base::ChannelGroupType::eInvalid)
            {
                auto channelConfig = YADAW::Entity::configFromGroupType(channelGroupType);
                if(const auto& channels = bus["channels"];
                    channels.IsSequence())
                {
                    auto channelCount = channels.size();
                    model.append(channelConfig);
                    model.setName(i, name);
                    for(decltype(channelCount) j = 0; j < channelCount; ++j)
                    {
                        const auto& channel = channels[j];
                        auto deviceIndex = channel["device-index"].as<std::uint32_t>(YADAW::Audio::Device::InvalidIndex);
                        auto channelIndex = channel["channel-index"].as<std::uint32_t>(YADAW::Audio::Device::InvalidIndex);
                        model.setChannel(i, j, deviceIndex, channelIndex);
                    }
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
    FOR_RANGE0(i, channelCount)
    {
        YAMLMap map(emitter);
        const auto& channel = bus.channelAt(i).value();
        emitter << YAML::Key << "device-index" << YAML::Value << channel.deviceIndex;
        emitter << YAML::Key << "channel-index" << YAML::Value << channel.channelIndex;
    }
}

void loadAudioBusConfiguration(const YAML::Node& node,
    YADAW::Model::AudioBusConfigurationModel& inputModel,
    YADAW::Model::AudioBusConfigurationModel& outputModel)
{
    loadAudioBusConfiguration(node["input-buses"], inputModel);
    loadAudioBusConfiguration(node["output-buses"], outputModel);
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
        FOR_RANGE0(i, inputCount)
        {
            YAMLMap map1(emitter);
            const auto& bus = configuration.inputBusAt(i)->get();
            emitter << YAML::Key << "name" << YAML::Value
                << inputModel.nameAt(i)->get().toStdString();
            emitter << YAML::Key << "channel-config" << YAML::Value
                << channelConfigNames[YADAW::Util::underlyingValue(bus.channelGroupType()) + 2];
            emitAudioChannelList(emitter, bus);
        }
    }
    emitter << YAML::Key << "output-buses" << YAML::Value;
    {
        YAMLSeq seq(emitter);
        auto outputCount = configuration.outputBusCount();
        FOR_RANGE0(i, outputCount)
        {
            YAMLMap map1(emitter);
            const auto& bus = configuration.outputBusAt(i)->get();
            emitter << YAML::Key << "name" << YAML::Value
                << outputModel.nameAt(i)->get().toStdString();
            emitter << YAML::Key << "channel-config" << YAML::Value
                    << channelConfigNames[YADAW::Util::underlyingValue(bus.channelGroupType()) + 2];
            emitAudioChannelList(emitter, bus);
        }
    }
    return YAML::Load(emitter.c_str());
}
}