#ifndef YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER

#include "audio/plugin/PluginWindow.hpp"
#include "controller/PluginContext.hpp"

#include <QWindow>

#include <map>

namespace YADAW::Controller
{
void createPluginWindows(PluginContext& context);
}

#endif // YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
