#include "EntityInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "entity/AudioBackendSupport.hpp"
#include "entity/IAudioIOPosition.hpp"
#include "entity/ChannelConfig.hpp"
#include "entity/PluginFormatSupport.hpp"

namespace YADAW::Entity
{
QString entityIsInterfaceText = "This model is an abstract entity instantiated in C++ codes.";
void initializeEntity()
{
    QML_REGISTER_TYPE(AudioBackendSupport, YADAW.Entities, 1, 0);
    QML_REGISTER_TYPE(ChannelConfig,       YADAW.Entities, 1, 0);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioIOPosition, YADAW.Entities, 1, 0, entityIsInterfaceText);
    QML_REGISTER_TYPE(PluginFormatSupport, YADAW.Entities, 1, 0);
}
}