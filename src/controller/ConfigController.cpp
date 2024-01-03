#include "ConfigController.hpp"

#include "entity/AudioBackendSupport.hpp"
#if __linux__
#include "controller/ALSABackendController.hpp"
#endif
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
        auto backend = YADAW::Entity::AudioBackendSupport::Backend::Off;
        node_["general"]["language"] = "English";
        node_["general"]["system-font-rendering"] = false;
        node_["general"]["system-font-rendering-while-debugging"] = false;
#if _WIN32
        backend = YADAW::Entity::AudioBackendSupport::Backend::AudioGraph;
#elif __linux__
        backend = YADAW::Entity::AudioBackendSupport::Backend::ALSA;
        node_["audio-hardware"]["alsa"]["sample-rate"] = YADAW::Controller::DefaultSampleRate;
        node_["audio-hardware"]["alsa"]["buffer-size"] = YADAW::Controller::DefaultFrameSize;
#endif
        node_["audio-hardware"]["audio-api"] = YADAW::Entity::AudioBackendSupport::backendNames[backend];
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