#include "EntityInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "entity/AudioBackendSupport.hpp"
#include "entity/ChannelConfig.hpp"
#include "entity/PluginFormatSupport.hpp"

namespace YADAW::Entity
{
void initializeEntity()
{
    QML_REGISTER_TYPE(AudioBackendSupport, YADAW.Entities, 1, 0);
    QML_REGISTER_TYPE(ChannelConfig, YADAW.Entities, 1, 0);
    QML_REGISTER_TYPE(PluginFormatSupport, YADAW.Entities, 1, 0);
}
}