#include "PluginPoolController.hpp"

namespace YADAW::Controller
{
PluginPool& appPluginPool()
{
    static PluginPool ret;
    return ret;
}
}