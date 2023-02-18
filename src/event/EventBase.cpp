#include "EventBase.hpp"

namespace YADAW::Event
{
QObject* eventSender = nullptr;
QObject* eventReceiver = nullptr;
EventHandler* eventHandler = nullptr;
SplashScreenWorkerThread* splashScreenWorkerThread = nullptr;
}