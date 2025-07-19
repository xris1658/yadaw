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
    enum DragPosition
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
    bool aboutToStartResize_;
    bool resizing_ = false;
};
}

#endif // YADAW_SRC_UI_RESIZEEVENTFILTER