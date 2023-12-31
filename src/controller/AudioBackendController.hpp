#ifndef YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER

#include "entity/AudioBackendSupport.hpp"

namespace YADAW::Controller
{
YADAW::Entity::AudioBackendSupport::Backend backendFromConfig(const YAML::Node& node);

void saveBackendToConfig(YADAW::Entity::AudioBackendSupport::Backend backend,
    YAML::Node& node);
}

#endif //YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
