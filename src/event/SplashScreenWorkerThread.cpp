#include "SplashScreenWorkerThread.hpp"

#include "event/EventBase.hpp"

namespace YADAW::Event
{
SplashScreenWorkerThread::SplashScreenWorkerThread(QObject* splashScreenEvents,
    QObject* parent):
    QThread(parent)
{
    QObject::connect(this, SIGNAL(setText(QString)),
        splashScreenEvents, SIGNAL(setText(QString)),
        Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, SIGNAL(closeSplashScreen()),
        splashScreenEvents, SIGNAL(closeSplashScreen()),
        Qt::ConnectionType::QueuedConnection);
}

void SplashScreenWorkerThread::run()
{
    YADAW::Event::eventHandler->onStartInitializingApplication();
}
}