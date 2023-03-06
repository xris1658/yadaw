#include "EventHandler.hpp"

#include "controller/AppController.hpp"
#include "controller/AssetDirectoryController.hpp"
#include "controller/PluginDirectoryController.hpp"
#include "event/EventBase.hpp"
#include "native/Native.hpp"
#include "native/WindowsDarkModeSupport.hpp"
#include "ui/UI.hpp"

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
}

void EventHandler::connectToEventReceiver(QObject* receiver)
{
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        receiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(QString)),
        receiver, SIGNAL(setQtVersion(QString)));
    QObject::connect(this, SIGNAL(setSplashScreenText(QString)),
        receiver, SIGNAL(setSplashScreenText(QString)));
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
}
