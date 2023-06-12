#ifndef YADAW_SRC_EVENT_EVENTBASE
#define YADAW_SRC_EVENT_EVENTBASE

#include "event/EventHandler.hpp"
#include "event/SplashScreenWorkerThread.hpp"

#include <QObject>

namespace YADAW::Event
{
extern QObject* eventSender;
extern QObject* eventReceiver;
extern EventHandler* eventHandler;
extern SplashScreenWorkerThread* splashScreenWorkerThread;
}

#endif // YADAW_SRC_EVENT_EVENTBASE
