#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINCONTROLLER

#include "audio/engine/PluginNode.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/plugin/VST3Plugin.hpp"

namespace YADAW::Controller
{
using namespace YADAW::Audio::Engine;
using namespace YADAW::Audio::Plugin;

PluginNode createNodeFromCLAP(std::shared_ptr<CLAPPlugin> plugin);

PluginNode createNodeFromVST3(std::shared_ptr<VST3Plugin> plugin);
}

#endif //YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
