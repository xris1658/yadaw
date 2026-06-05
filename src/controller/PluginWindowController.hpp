#ifndef YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER

#include "audio/plugin/PluginWindow.hpp"
#include "controller/PluginContext.hpp"

#include <QWindow>

#include <map>

namespace YADAW::Controller
{
void createPluginWindows(PluginContext& context);

bool resizeFromVST3Plugin(QWindow& window, const QSize& size);

bool requestResizeFromCLAPPlugin(YADAW::Audio::Plugin::CLAPPlugin& plugin, const QSize& size);
}

#endif // YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
