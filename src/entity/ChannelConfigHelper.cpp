#include "ChannelConfigHelper.hpp"

#include "util/Base.hpp"

namespace YADAW::Entity
{
YADAW::Audio::Base::ChannelGroupType groupTypeFromConfig(YADAW::Entity::ChannelConfig::Config config)
{
    return static_cast<YADAW::Audio::Base::ChannelGroupType>(
        static_cast<int>(config) - 1
    );
}

YADAW::Entity::ChannelConfig::Config configFromGroupType(YADAW::Audio::Base::ChannelGroupType groupType)
{
    return static_cast<YADAW::Entity::ChannelConfig::Config>(
        YADAW::Util::underlyingValue(groupType) + 1
    );
}
}