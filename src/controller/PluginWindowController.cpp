#include "PluginWindowController.hpp"

#include "native/Window.hpp"
#include "ui/Runtime.hpp"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVariant>

#include <mutex>

namespace YADAW::Controller
{
struct PluginWindows
{
    QWindow* pluginWindowTopBarFrame = nullptr;
    QWindow* genericEditorWindow = nullptr;
};

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
                    if(fileName == "PluginWindowTopBarFrame.qml")
                    {
                        pluginWindows.pluginWindowTopBarFrame = qobject_cast<QQuickWindow*>(obj);
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

void createGenericPluginEditor()
{
    connectEngine();
    YADAW::UI::qmlApplicationEngine->loadFromModule("content", "GenericPluginEditor");
    pluginWindows.genericEditorWindow->setTransientParent(YADAW::UI::mainWindow);
}

void createPluginWindows(PluginContext& context)
{
    connectEngine();
    auto pluginNeedsWindow = &(context.pluginInstance.plugin()->get());
    if(auto pluginGUI = pluginNeedsWindow->gui())
    {
        context.editor = new YADAW::Audio::Plugin::PluginWindow();
        YADAW::UI::qmlApplicationEngine->loadFromModule("content", "PluginWindowTopBarFrame");
        context.editor->setTransientParent(YADAW::UI::mainWindow);
        context.editor->setGUI(*pluginGUI);
        context.editor->setTopBar(pluginWindows.pluginWindowTopBarFrame);
        context.editor->setCanClose(false);
        pluginWindows.pluginWindowTopBarFrame = nullptr;
    }
    createGenericPluginEditor();
    context.genericEditor = pluginWindows.genericEditorWindow;
    pluginWindows.genericEditorWindow = nullptr;
}
}