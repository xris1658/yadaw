#ifndef YADAW_SRC_UI_RAWMOUSEEVENTFILTER
#define YADAW_SRC_UI_RAWMOUSEEVENTFILTER

#include "WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>

namespace YADAW::UI
{
class RawMouseEventFilter: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    RawMouseEventFilter(QWindow& window);
    ~RawMouseEventFilter() override;
public:
    void beginHideCursor();
    void endHideCursor();
    void beginLockCursor(const QPoint& point);
    void endLockCursor();
    void beginHandlingRawMouseEvent();
    void endHandlingRawMouseEvent();
    bool isHandlingRawMouseEvent() const;
    QPoint rawCursorPosition() const;
public:
    bool eventFilter(QObject* obj, QEvent* event) override;
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WindowAndId windowAndId_;
    bool handlingRawMouseEvent_ = false;
    std::uint32_t prevX_;
    std::uint32_t prevY_;
};
}

#endif // YADAW_SRC_UI_RAWMOUSEEVENTFILTER