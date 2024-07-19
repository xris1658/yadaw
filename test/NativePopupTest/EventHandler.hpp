#ifndef YADAW_TEST_NATIVEPOPUPTEST_EVENTHANDLER
#define YADAW_TEST_NATIVEPOPUPTEST_EVENTHANDLER

#include <QObject>
#include <QWindow>

class EventHandler: public QObject
{
    Q_OBJECT
public:
    EventHandler(QWindow& mainWindow);
    ~EventHandler() override;
public slots:
    void onShowNativePopup(int x, int y);
private:
    QWindow* mainWindow_;
};

#endif // YADAW_TEST_NATIVEPOPUPTEST_EVENTHANDLER
