#ifndef YADAW_SRC_DAO_CONFIG
#define YADAW_SRC_DAO_CONFIG

#include <QString>

#include <yaml-cpp/yaml.h>

namespace YADAW::DAO
{
const QString& configFilePath();

void saveConfig(const YAML::Node& node, const QString& path);

YAML::Node loadConfig(const QString& path);
}

#endif //YADAW_SRC_DAO_CONFIG
