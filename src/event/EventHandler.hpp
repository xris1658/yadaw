#ifndef YADAW_SRC_EVENT_EVENTHANDLER
#define YADAW_SRC_EVENT_EVENTHANDLER

#include <QObject>

namespace YADAW::Event
{
class EventHandler: public QObject
{
    Q_OBJECT
public:
    EventHandler(QObject* sender, QObject* receiver, QObject* parent = nullptr);
private:
    void connectToEventSender(QObject* sender);
    void connectToEventReceiver(QObject* receiver);
signals:
    void mainWindowCloseAccepted();
    void setQtVersion(const QString& version);
    void setSplashScreenText(const QString& text);
    void pluginScanComplete();
    void messageDialog(const QString& message, const QString& title, int icon, bool modal);
public slots:
    void onStartInitializingApplication();
    void onOpenMainWindow();
    void onMainWindowClosing();
    void onAddWindowForDarkModeSupport();
    void onRemoveWindowForDarkModeSupport();
    void onLocateFileInExplorer(const QString& path);
    void onStartPluginScan();
    void onSetSystemFontRendering(bool enabled);
private:
    QObject* eventSender_;
    QObject* eventReceiver_;
};
}

#endif //YADAW_SRC_EVENT_EVENTHANDLER
