#ifndef YADAW_TEST_COMMON_CLOSEWINDOWEVENTFILTER
#define YADAW_TEST_COMMON_CLOSEWINDOWEVENTFILTER

#include <QObject>
#include <QWindow>

class CloseWindowEventFilter: public QObject
{
    Q_OBJECT
public:
    CloseWindowEventFilter();
    ~CloseWindowEventFilter();
public:
    void setWindow(QWindow& window);
    void setClose(bool close);
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
signals:
    void aboutToClose();
private:
    QWindow* window_ = nullptr;
    bool close_ = false;
};

#endif // YADAW_TEST_COMMON_CLOSEWINDOWEVENTFILTER
