#ifndef YADAW_SRC_EVENT_SPLASHSCREENWORKERTHREAD
#define YADAW_SRC_EVENT_SPLASHSCREENWORKERTHREAD

#include <QObject>
#include <QThread>

namespace YADAW::Event
{
// To prevent
class SplashScreenWorkerThread: public QThread
{
    Q_OBJECT
public:
    SplashScreenWorkerThread(QObject* splashScreenEvents, QObject* parent = nullptr);
    ~SplashScreenWorkerThread() = default;
signals:
    void setText(const QString& text);
    void openMainWindow();
    void closeSplashScreen();
protected:
    void run() override;
};
}

#endif // YADAW_SRC_EVENT_SPLASHSCREENWORKERTHREAD
