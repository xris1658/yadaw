#include "PluginPositionMap.hpp"

namespace YADAW::Controller
{
PluginPositionMap& appPluginPosition()
{
    static PluginPositionMap ret;
    return ret;
}
}