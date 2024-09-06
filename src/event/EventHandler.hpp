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
    void mainWindowReady();
    void mainWindowCloseAccepted();
    void setQtVersion(int majorVersion, int miinorVersion, int microVersion);
    void setQtCopyrightYear(const QString& copoyrightYear);
    void setFileBrowserName(const QString& fileBrowsreName);
    void setSplashScreenText(const QString& text);
    void pluginScanComplete();
    void setScanningDirectories(bool value);
    void setPluginScanTotalCount(int count);
    void setPluginScanProgress(int count, const QString& currentName);
public slots:
    void onStartInitializingApplication();
    void onOpenMainWindow();
    void onMainWindowClosing();
    void onLocateFileInExplorer(const QString& path);
    void onStartPluginScan();
    void onSetSystemFontRendering(bool enabled);
    void onSetSystemFontRenderingWhileDebugging(bool enabled);
    void onSetTranslationIndex(int index);
    void onCurrentAudioBackendChanged();
    void onAudioGraphOutputDeviceIndexChanged(int index);
    void onSetMainWindowFromMaximizedToFullScreen();
    void onSetMainWindowFromFullScreenToMaximized();
private:
    QObject* eventSender_;
    QObject* eventReceiver_;
};
}

#endif // YADAW_SRC_EVENT_EVENTHANDLER
