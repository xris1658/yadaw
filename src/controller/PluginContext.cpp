#include "PluginContext.hpp"

namespace YADAW::Controller
{
PluginContexts& appPluginContexts()
{
    static PluginContexts ret;
    return ret;
}
}