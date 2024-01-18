#include "entity/ChannelConfig.hpp"

#include "audio/device/IAudioChannelGroup.hpp"
#include "entity/ChannelConfigHelper.hpp"

namespace YADAW::Entity
{
class AudioChannelGroupCount: public YADAW::Audio::Device::IAudioChannelGroup
{
public:
    static int channelCount(YADAW::Audio::Base::ChannelGroupType channelGroupType)
    {
        return IAudioChannelGroup::channelCount(channelGroupType);
    }
};

ChannelConfig::ChannelConfig(QObject* parent): QObject(parent) {}

int ChannelConfig::channelCount(ChannelConfig::Config config)
{
    return YADAW::Audio::Device::IAudioChannelGroup::channelCount(
        YADAW::Entity::groupTypeFromConfig(config)
    );
}
}