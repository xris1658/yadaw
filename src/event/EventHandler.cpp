#include "EventHandler.hpp"

#include "controller/AppController.hpp"
#include "event/EventBase.hpp"
#include "native/WindowsDarkModeSupport.hpp"
#include "ui/UI.hpp"

#include <thread>

namespace YADAW::Event
{
EventHandler::EventHandler(QObject* eventSender, QObject* eventReceiver, QObject* parent):
    QObject(parent)
{
    connectToEventSender(eventSender);
    connectToEventReceiver(eventReceiver);
}

void EventHandler::connectToEventSender(QObject* eventSender)
{
    QObject::connect(eventSender, SIGNAL(startInitializingApplication()),
        this, SLOT(onStartInitializingApplication()));
    QObject::connect(eventSender, SIGNAL(mainWindowClosing()),
        this, SLOT(onMainWindowClosing()));
}

void EventHandler::connectToEventReceiver(QObject* eventReceiver)
{
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        eventReceiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(QString)),
        eventReceiver, SIGNAL(setQtVersion(QString)));
    QObject::connect(this, SIGNAL(setSplashScreenText(QString)),
        eventReceiver, SIGNAL(setSplashScreenText(QString)));
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
    YADAW::Event::splashScreenWorkerThread->closeSplashScreen();
}

void EventHandler::onMainWindowClosing()
{
    YADAW::Native::WindowsDarkModeSupport::instance().reset();
    mainWindowCloseAccepted();
}
}
