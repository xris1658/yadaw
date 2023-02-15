#include "EventHandler.hpp"

#include "native/WindowsDarkModeSupport.hpp"

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
    QObject::connect(eventSender, SIGNAL(mainWindowClosing()),
        this, SLOT(onMainWindowClosing()));
}

void EventHandler::connectToEventReceiver(QObject* eventReceiver)
{
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        eventReceiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(QString)),
        eventReceiver, SIGNAL(setQtVersion(QString)));
}

void EventHandler::onMainWindowClosing()
{
    YADAW::Native::WindowsDarkModeSupport::instance().reset();
    mainWindowCloseAccepted();
}
}