#include "VST3PluginPool.hpp"

#include "audio/plugin/VST3Plugin.hpp"

namespace YADAW::Controller
{
VST3PluginPool& appVST3PluginPool()
{
    static VST3PluginPool ret;
    return ret;
}

VST3PluginPoolVector createPoolVector(VST3PluginPool& pool)
{
    VST3PluginPoolVector ret;
    ret.reserve(pool.size());
    for(auto it = pool.begin(); it != pool.end(); ++it)
    {
        ret.emplace_back(&*it);
    }
    return ret;
}

void fillVST3InputParameterChanges(VST3PluginPoolVector& pool,
    std::int64_t callbackTimestampInNanosecond)
{
    for(auto* ptr: pool)
    {
        auto& [plugin, context] = *ptr;
        context.componentHandler->switchBuffer(
            callbackTimestampInNanosecond
        );
    }
}
}