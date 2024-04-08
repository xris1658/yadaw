#ifndef YADAW_SRC_ENTITY_CHANNELCONFIGHELPER
#define YADAW_SRC_ENTITY_CHANNELCONFIGHELPER

#include "audio/base/Channel.hpp"
#include "entity/ChannelConfig.hpp"

namespace YADAW::Entity
{
YADAW::Audio::Base::ChannelGroupType groupTypeFromConfig(YADAW::Entity::ChannelConfig::Config config);

YADAW::Entity::ChannelConfig::Config configFromGroupType(YADAW::Audio::Base::ChannelGroupType groupType);
}

#endif //YADAW_SRC_ENTITY_CHANNELCONFIGHELPER
