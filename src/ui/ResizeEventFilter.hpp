#ifndef YADAW_SRC_UI_RESIZEEVENTFILTER
#define YADAW_SRC_UI_RESIZEEVENTFILTER

#include "WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>

namespace YADAW::UI
{
class ResizeEventFilter: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    enum DragPosition: std::uint8_t
    {
        TopLeft,
        Top,
        TopRight,
        Left,
        Right,
        BottomLeft,
        Bottom,
        BottomRight
    };
public:
    ResizeEventFilter(QWindow& window);
    ~ResizeEventFilter() override;
signals:
    void startResize();
    void aboutToResize(DragPosition dragPosition, QRect* rect);
    void resized();
    void endResize();
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WindowAndId windowAndId_;
    DragPosition position_;
    bool aboutToStartResize_ = false;
    bool resizing_ = false;
#if _WIN32
    bool prevIsCaptureChanged_ = false;
#endif
};
}

#endif // YADAW_SRC_UI_RESIZEEVENTFILTER