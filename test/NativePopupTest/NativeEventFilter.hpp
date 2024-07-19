#ifndef YADAW_TEST_NATIVEPOPUPTEST_NATIVEEVENTFILTER
#define YADAW_TEST_NATIVEPOPUPTEST_NATIVEEVENTFILTER

#include <QAbstractNativeEventFilter>
#include <QWindow>

class NativeEventFilter: public QAbstractNativeEventFilter
{
public:
    NativeEventFilter(QWindow& nativePopup);
    ~NativeEventFilter() override;
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WId wid_;
};

#endif // YADAW_TEST_NATIVEPOPUPTEST_NATIVEEVENTFILTER
