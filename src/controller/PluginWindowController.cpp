#include "PluginWindowController.hpp"

#include "native/Window.hpp"
#include "ui/Runtime.hpp"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVariant>

#include <mutex>

namespace YADAW::Controller
{
YADAW::Audio::Plugin::IAudioPlugin* pluginNeedsWindow = nullptr;

PluginWindows pluginWindows;

std::once_flag connectEngineFlag;

void connectEngine()
{
    std::call_once(
        connectEngineFlag,
        []()
        {
            QObject::connect(
                YADAW::UI::qmlApplicationEngine,
                &QQmlApplicationEngine::objectCreated,
                [](QObject *obj, const QUrl &objUrl)
                {
                    const auto& fileName = objUrl.fileName();
                    if(fileName == "PluginWindow.qml")
                    {
                        pluginWindows.pluginWindow = qobject_cast<QWindow*>(obj);
                    }
                    else if(fileName == "GenericPluginEditor.qml")
                    {
                        pluginWindows.genericEditorWindow = qobject_cast<QWindow*>(obj);
                    }
                }
            );
        }
    );
}

void createPluginWindow()
{
    connectEngine();
    YADAW::UI::qmlApplicationEngine->loadFromModule("content", "PluginWindow");
    auto pluginWindow = pluginWindows.pluginWindow;
    auto pluginFrame = pluginWindow->property("pluginFrame").value<QWindow*>();
    auto gui = pluginNeedsWindow->gui();
    gui->attachToWindow(pluginFrame);
    if(!gui->resizableByUser())
    {
        YADAW::Native::setWindowResizable(*pluginFrame, false);
        YADAW::Native::setWindowResizable(*pluginWindow, false);
    }
    // Embed the plugin frame to its outer window (See `PluginWindow.qml`)
    // This process has to be done AFTER `IPluginGUI::attachToWindow`, or the
    // initial size of the GUI would be incorrect.
    pluginFrame->setParent(pluginWindow);
    pluginWindow->setTransientParent(YADAW::UI::mainWindow);
}

void createGenericPluginEditor()
{
    connectEngine();
    YADAW::UI::qmlApplicationEngine->loadFromModule("content", "GenericPluginEditor");
    pluginWindows.genericEditorWindow->setTransientParent(YADAW::UI::mainWindow);
}

void createPluginWindows(PluginContext& context)
{
    connectEngine();
    pluginNeedsWindow = &(context.pluginInstance.plugin()->get());
    createPluginWindow();
    createGenericPluginEditor();
    context.editor = pluginWindows.pluginWindow;
    context.genericEditor = pluginWindows.genericEditorWindow;
}
}