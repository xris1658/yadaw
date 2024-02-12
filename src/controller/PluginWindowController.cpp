#include "PluginWindowController.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Plugin::IAudioPlugin* pluginNeedsWindow = nullptr;

PluginWindowPool& appPluginWindowPool()
{
    static PluginWindowPool ret;
    return ret;
}
}