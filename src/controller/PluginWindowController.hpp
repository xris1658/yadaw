#ifndef YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER

#include "audio/plugin/IAudioPlugin.hpp"
#include "controller/PluginContext.hpp"

#include <QWindow>

#include <map>

namespace YADAW::Controller
{
struct PluginWindows
{
    QWindow* pluginWindow = nullptr;
    QWindow* genericEditorWindow = nullptr;
};

extern YADAW::Audio::Plugin::IAudioPlugin* pluginNeedsWindow;

extern PluginWindows pluginWindows;

void createPluginWindow();

void createGenericPluginEditor();

void createPluginWindows(PluginContext& context);
}

#endif // YADAW_SRC_CONTROLLER_PLUGINWINDOWCONTROLLER
