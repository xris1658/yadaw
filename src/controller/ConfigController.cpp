#include "ConfigController.hpp"

#include "dao/Config.hpp"
#include "dao/DAOBase.hpp"

#include <QFileInfo>

namespace YADAW::Controller
{
namespace Impl
{
class YAMLNodeInitializer
{
private:
    YAMLNodeInitializer()
    {
        node_["general"]["language"] = "en_US";
        node_["general"]["system-font-rendering"] = false;
#if(WIN32)
        node_["audio-hardware"]["audio-api"] = "AudioGraph";
#elif(__linux__)
        node_["audio-hardware"]["audio-api"] = "ALSA";
#endif
        node_["plugin"]["scan-shortcuts"] = false;
        node_["plugin"]["scan-on-startup"] = false;
    }

public:
    static YAMLNodeInitializer& instance()
    {
        static YAMLNodeInitializer ret;
        return ret;
    }

    YAML::Node& node()
    { return node_; }

private:
    YAML::Node node_;
};
}

const YAML::Node& defaultConfig()
{
    return Impl::YAMLNodeInitializer::instance().node();
}

void initializeApplicationConfig()
{
    if(!QFileInfo::exists(YADAW::DAO::appConfigPath()))
    {
        YADAW::DAO::saveConfig(defaultConfig(), YADAW::DAO::appConfigPath());
    }
}

YAML::Node loadConfig()
{
    return YADAW::DAO::loadConfig(YADAW::DAO::appConfigPath());
}

void saveConfig(const YAML::Node& config)
{
    YADAW::DAO::saveConfig(config, YADAW::DAO::appConfigPath());
}
}