#ifndef YADAW_SRC_CONTROLLER_VST3PLUGINPOOL
#define YADAW_SRC_CONTROLLER_VST3PLUGINPOOL

#include "audio/host/VST3ComponentHandler.hpp"

#include <map>

namespace YADAW::Audio::Plugin
{
class VST3Plugin;
}

namespace YADAW::Controller
{
struct VST3PluginContext
{
    YADAW::Audio::Host::VST3ComponentHandler* componentHandler;
};

using VST3PluginPool = std::map<
    YADAW::Audio::Plugin::VST3Plugin*,
    VST3PluginContext
>;

using VST3PluginPoolVector = std::vector<
    VST3PluginPool::value_type*
>;

VST3PluginPool& appVST3PluginPool();

VST3PluginPoolVector createPoolVector(VST3PluginPool& pool);

void fillVST3InputParameterChanges(VST3PluginPoolVector& pool,
    std::int64_t callbackTimestampInNanosecond);
}

#endif // YADAW_SRC_CONTROLLER_VST3PLUGINPOOL
