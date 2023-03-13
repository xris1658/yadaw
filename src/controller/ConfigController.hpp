#ifndef YADAW_SRC_CONTROLLER_CONFIGCONTROLLER
#define YADAW_SRC_CONTROLLER_CONFIGCONTROLLER

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
const YAML::Node& defaultConfig();

void initializeApplicationConfig();

YAML::Node loadConfig();

void saveConfig(const YAML::Node& config);
}

#endif //YADAW_SRC_CONTROLLER_CONFIGCONTROLLER
