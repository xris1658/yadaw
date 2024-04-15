#include "PluginWindowController.hpp"

#include "ui/UI.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Plugin::IAudioPlugin* pluginNeedsWindow = nullptr;

PluginWindows pluginWindows;

void createPluginWindow()
{
    YADAW::UI::qmlApplicationEngine->loadFromModule("content", "PluginWindow");
    auto pluginWindow = pluginWindows.pluginWindow;
    auto pluginFrame = pluginWindow->property("pluginFrame").value<QWindow*>();
    pluginNeedsWindow->gui()->attachToWindow(pluginFrame);
    // Embed the plugin frame to its outer window (See `PluginWindow.qml`)
    // This process has to be done AFTER `IPluginGUI::attachToWindow`, or the
    // initial size of the GUI would be incorrect.
    pluginFrame->setParent(pluginWindow);
    pluginWindow->setTransientParent(YADAW::UI::mainWindow);
}

void createGenericPluginEditor()
{
    YADAW::UI::qmlApplicationEngine->loadFromModule("content", "GenericPluginEditor");
    pluginWindows.genericEditorWindow->setTransientParent(YADAW::UI::mainWindow);
}
}