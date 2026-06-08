#include "PluginWindowController.hpp"

#include "native/Window.hpp"
#include "ui/Runtime.hpp"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVariant>

#include <mutex>

#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/plugin/VST3PluginGUI.hpp"

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
        if(auto format = pluginNeedsWindow->format(); format == YADAW::Audio::Plugin::PluginFormat::VST3)
        {
            static_cast<YADAW::Audio::Plugin::VST3PluginGUI*>(pluginGUI)->frame().setResizeInitiatedFromPluginCallback(
                &resizeFromVST3Plugin
            );
        }
        else if(format == YADAW::Audio::Plugin::PluginFormat::CLAP)
        {
            static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(pluginNeedsWindow)->host().setRequestResizeCallback(
                &requestResizeFromCLAPPlugin
            );
        }
        context.editor = new YADAW::Audio::Plugin::PluginWindow();
        YADAW::UI::qmlApplicationEngine->loadFromModule("content", "PluginWindowTopBarFrame");
        context.editor->setTransientParent(YADAW::UI::mainWindow);
        context.editor->setGUI(*pluginGUI);
        context.editor->setTopBar(pluginWindows.pluginWindowTopBarFrame);
        context.editor->setProperty("canClose", QVariant::fromValue<bool>(false));
#ifndef NDEBUG
        {
            auto vCanClose = context.editor->property("canClose");
            assert(vCanClose.isValid() && vCanClose.value<bool>() == false);
        }
#endif
        QObject::connect(
            context.editor, &YADAW::Audio::Plugin::PluginWindow::aboutToClose,
            [window = context.editor](bool& canClose)
            {
                canClose = window->property("close").value<bool>();
                if(!canClose)
                {
                    window->hide();
                }
            }
        );
        pluginWindows.pluginWindowTopBarFrame = nullptr;
    }
    createGenericPluginEditor();
    context.genericEditor = pluginWindows.genericEditorWindow;
    pluginWindows.genericEditorWindow = nullptr;
}

bool resizeFromVST3Plugin(QWindow& window, const QSize& size)
{
    auto pluginWindow = static_cast<YADAW::Audio::Plugin::PluginWindow*>(window.parent());
    pluginWindow->resizeFromPlugin(size);
    return true;
}

bool requestResizeFromCLAPPlugin(YADAW::Audio::Plugin::CLAPPlugin& plugin, const QSize& size)
{
    auto window = plugin.pluginGUI()->window();
    auto pluginWindow = static_cast<YADAW::Audio::Plugin::PluginWindow*>(window->parent());
    pluginWindow->resizeFromPlugin(size);
    return true;
}
}
