#include "entity/ChannelConfig.hpp"

#include "audio/device/IAudioChannelGroup.hpp"

namespace YADAW::Entity
{
ChannelConfig::ChannelConfig(QObject* parent): QObject(parent) {}

int ChannelConfig::channelCount(ChannelConfig::Config config)
{
    return YADAW::Audio::Device::IAudioChannelGroup::channelCount(
        static_cast<YADAW::Audio::Base::ChannelGroupType>(config - 1));
}
}