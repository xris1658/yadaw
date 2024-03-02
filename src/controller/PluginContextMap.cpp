#include "PluginContextMap.hpp"

namespace YADAW::Controller
{
PluginContextMap& appPluginContextMap()
{
    static PluginContextMap ret;
    return ret;
}
}