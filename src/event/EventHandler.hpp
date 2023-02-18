#ifndef YADAW_SRC_EVENT_EVENTHANDLER
#define YADAW_SRC_EVENT_EVENTHANDLER

#include <QObject>

namespace YADAW::Event
{
class EventHandler: public QObject
{
    Q_OBJECT
public:
    EventHandler(QObject* eventSender, QObject* eventReceiver, QObject* parent = nullptr);
private:
    void connectToEventSender(QObject* eventSender);
    void connectToEventReceiver(QObject* eventReceiver);
public slots:
    void onStartInitializingApplication();
    void onOpenMainWindow();
    void onMainWindowClosing();
signals:
    void mainWindowCloseAccepted();
    void setQtVersion(const QString& version);
    void setSplashScreenText(const QString& text);
};
}

#endif //YADAW_SRC_EVENT_EVENTHANDLER
