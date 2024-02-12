#ifndef YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER

#include "audio/plugin/IAudioPlugin.hpp"

#include <QWindow>

#include <map>

namespace YADAW::Controller
{
struct PluginWindows
{
    QWindow* pluginWindow = nullptr;
    QWindow* genericEditorWindow = nullptr;
};

using PluginWindowPool = std::map<
    YADAW::Audio::Plugin::IAudioPlugin*,
    PluginWindows
>;

PluginWindowPool& appPluginWindowPool();

extern YADAW::Audio::Plugin::IAudioPlugin* pluginNeedsWindow;
}

#endif // YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
