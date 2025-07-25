#ifndef YADAW_SRC_UI_RESIZEEVENTFILTER
#define YADAW_SRC_UI_RESIZEEVENTFILTER

#include "WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>

#if __linux__
#include <xcb/xcb.h>
#endif

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
    enum FeatureSupportFlag: std::uint64_t
    {
        SupportsStartStopResize       = 1 << 0,
        SupportsAboutToResize         = 1 << 1,
        SupportsDragPosition          = 1 << 2,
        SupportsResized               = 1 << 3,
        SupportsAdjustOnAboutToResize = 1 << 4
    };
    using FeatureSupportFlags = std::underlying_type_t<FeatureSupportFlag>;
public:
    ResizeEventFilter(QWindow& window);
    ~ResizeEventFilter() override;
public:
    static FeatureSupportFlags getNativeSupportFlags();
    // Valid if `SupportsStartStopResize`; always return false otherwise
    bool resizing() const;
signals:
    // Emitted if `SupportsStartStopResize`
    void startResize();
    // Emitted if `SupportsAboutToResize`
    // - `dragPosition` is valid if `SupportsDragPosition`
    // - `*rect` can be adjusted if `SupportsAdjustOnAboutToResize`
    void aboutToResize(DragPosition dragPosition, QRect* rect);
    // Emitted if `SupportsResized`
    void resized(QRect rect);
    // Emitted if `SupportsStartStopResize`
    void endResize();
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
#if __linux__
private:
    using DesktopNativeEventFilter = bool(ResizeEventFilter::*)(xcb_generic_event_t* event);
    bool nativeEventFilterOnKDE(xcb_generic_event_t* event);
    bool nativeEventFilterOnGNOME(xcb_generic_event_t* event);
    bool nativeEventFilterOnUnknown(xcb_generic_event_t* event);
#endif
private:
    WindowAndId windowAndId_;
    DragPosition position_;
    bool aboutToStartResize_ = false;
    bool resizing_ = false;
#if _WIN32
    bool prevIsCaptureChanged_ = false;
#elif __linux__
    static DesktopNativeEventFilter desktopNativeEventFilter;
    int lastResponseType_ = XCB_GE_GENERIC + 1;
#endif
};

void adjustRect(QRect& rect, ResizeEventFilter::DragPosition position, QSize newSize);
}

#endif // YADAW_SRC_UI_RESIZEEVENTFILTER