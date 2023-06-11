#include "EntityInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "entity/AudioBackendSupport.hpp"

namespace YADAW::Entity
{
void initializeEntity()
{
    QML_REGISTER_TYPE(AudioBackendSupport, YADAW.Entities, 1, 0);
}
}