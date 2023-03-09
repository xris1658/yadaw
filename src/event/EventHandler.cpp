#include "EventHandler.hpp"

#include "controller/AppController.hpp"
#include "controller/AssetDirectoryController.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginDirectoryController.hpp"
#include "controller/PluginListController.hpp"
#include "event/EventBase.hpp"
#include "native/Native.hpp"
#include "native/WindowsDarkModeSupport.hpp"
#include "ui/UI.hpp"

#include <QDir>

#include <thread>

namespace YADAW::Event
{
EventHandler::EventHandler(QObject* sender, QObject* receiver, QObject* parent):
    QObject(parent), eventSender_(sender), eventReceiver_(receiver)
{
    connectToEventSender(sender);
    connectToEventReceiver(receiver);
}

void EventHandler::connectToEventSender(QObject* sender)
{
    QObject::connect(sender, SIGNAL(startInitializingApplication()),
        this, SLOT(onStartInitializingApplication()));
    QObject::connect(sender, SIGNAL(mainWindowClosing()),
        this, SLOT(onMainWindowClosing()));
    QObject::connect(sender, SIGNAL(addWindowForDarkModeSupport()),
        this, SLOT(onAddWindowForDarkModeSupport()));
    QObject::connect(sender, SIGNAL(removeWindowForDarkModeSupport()),
        this, SLOT(onRemoveWindowForDarkModeSupport()));
    QObject::connect(sender, SIGNAL(locatePathInExplorer(QString)),
        this, SLOT(onLocateFileInExplorer(QString)));
    QObject::connect(sender, SIGNAL(startPluginScan()),
        this, SLOT(onStartPluginScan()));
}

void EventHandler::connectToEventReceiver(QObject* receiver)
{
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        receiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(QString)),
        receiver, SIGNAL(setQtVersion(QString)));
    QObject::connect(this, SIGNAL(setSplashScreenText(QString)),
        receiver, SIGNAL(setSplashScreenText(QString)));
    QObject::connect(this, SIGNAL(pluginScanComplete()),
        receiver, SIGNAL(pluginScanComplete()));
}

void EventHandler::onStartInitializingApplication()
{
    YADAW::Event::splashScreenWorkerThread->setText("Initializing application...");
    YADAW::Controller::initApplication();
    YADAW::Event::splashScreenWorkerThread->setText("Opening main window...");
    YADAW::Event::splashScreenWorkerThread->openMainWindow();
}

void EventHandler::onOpenMainWindow()
{
    const QUrl mainWindowUrl(u"qrc:Main/YADAW.qml"_qs);
    YADAW::UI::qmlApplicationEngine->load(mainWindowUrl);
    YADAW::UI::mainWindow->setProperty("assetDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAssetDirectoryListModel()));
    YADAW::UI::mainWindow->setProperty("pluginDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appPluginDirectoryListModel()));
    YADAW::UI::mainWindow->setProperty("pluginListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appPluginListModel()));
    YADAW::UI::mainWindow->setProperty("midiEffectListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appMIDIEffectListModel()));
    YADAW::UI::mainWindow->setProperty("instrumentListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appInstrumentListModel()));
    YADAW::UI::mainWindow->setProperty("audioEffectListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioEffectListModel()));
    setQtVersion(qVersion());
    YADAW::Event::splashScreenWorkerThread->closeSplashScreen();
}

void EventHandler::onMainWindowClosing()
{
    YADAW::Native::WindowsDarkModeSupport::instance().reset();
    mainWindowCloseAccepted();
}

void EventHandler::onAddWindowForDarkModeSupport()
{
    QObject* object = eventSender_->property("darkModeSupportWindow").value<QObject*>();
    YADAW::Native::WindowsDarkModeSupport::instance()->addWindow(qobject_cast<QWindow*>(object));
}

void EventHandler::onRemoveWindowForDarkModeSupport()
{
    QObject* object = eventSender_->property("darkModeSupportWindow").value<QObject*>();
    YADAW::Native::WindowsDarkModeSupport::instance()->removeWindow(qobject_cast<QWindow*>(object));
}

void EventHandler::onLocateFileInExplorer(const QString& path)
{
    YADAW::Native::locateFileInExplorer(path);
}

void EventHandler::onStartPluginScan()
{
    std::thread([this]()
    {
        YADAW::DAO::removeAllPluginCategories();
        YADAW::DAO::removeAllPlugins();
        YADAW::Controller::appPluginListModel().clear();
        YADAW::Controller::appMIDIEffectListModel().clear();
        YADAW::Controller::appInstrumentListModel().clear();
        YADAW::Controller::appAudioEffectListModel().clear();
        const auto& model = YADAW::Controller::appPluginDirectoryListModel();
        auto itemCount = model.itemCount();
        std::vector<std::vector<QString>> libLists;
        for(decltype(itemCount) i = 0; i < itemCount; ++i)
        {
            QDir dir(model[i]);
            if(dir.exists())
            {
                libLists.emplace_back(YADAW::Controller::scanDirectory(dir, true, true/*FIXME*/));
            }
        }
        for(auto& libList: libLists)
        {
            for(auto& lib: libList)
            {
                lib = QDir::toNativeSeparators(lib);
                const auto& results = YADAW::Controller::scanSingleLibraryFile(lib);
                for(const auto& result: results)
                {
                    YADAW::Controller::savePluginScanResult(result);
                }
            }
        }
        pluginScanComplete();
        YADAW::Controller::appPluginListModel().update();
        YADAW::Controller::appMIDIEffectListModel().update();
        YADAW::Controller::appInstrumentListModel().update();
        YADAW::Controller::appAudioEffectListModel().update();
    }).detach();
}
}
