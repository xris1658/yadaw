#include "CLAPPluginPool.hpp"

#include "audio/plugin/CLAPPlugin.hpp"

namespace YADAW::Controller
{
CLAPPluginPool& appCLAPPluginPool()
{
    static CLAPPluginPool ret;
    return ret;
}

CLAPPluginPoolVector createPoolVector(CLAPPluginPool& pool)
{
    CLAPPluginPoolVector ret;
    ret.reserve(pool.size());
    for(auto it = pool.begin(); it != pool.end(); ++it)
    {
        ret.emplace_back(&*it);
    }
    return ret;
}

void fillCLAPInputParameterChanges(CLAPPluginPoolVector& pool,
    std::int64_t callbackTimestampInNanosecond)
{
    for(auto* ptr: pool)
    {
        auto& [plugin, context] = *ptr;
        auto eventList = context.eventList;
        eventList->flipped();
        eventList->attachToProcessData(plugin->processData());
    }
}
}