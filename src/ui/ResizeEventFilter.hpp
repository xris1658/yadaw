#ifndef YADAW_SRC_UI_RESIZEEVENTFILTER
#define YADAW_SRC_UI_RESIZEEVENTFILTER

#include "WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>

#if _WIN32
#define NOMINMAX
#include <windows.h>
#elif __linux__
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
        BottomRight,
        Invalid = 0x7f
    };
    enum FeatureSupportFlag: std::uint64_t
    {
        SupportsStartAndEndResize     = 1 << 0,
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
    // Valid if `SupportsStartAndEndResize`; always return false otherwise
    bool resizing() const;
signals:
    // Emitted if `SupportsStartAndEndResize`
    void startResize();
    // Emitted if `SupportsAboutToResize`
    // - `dragPosition` is valid if `SupportsDragPosition`
    // - `*rect` can be adjusted if `SupportsAdjustOnAboutToResize`
    void aboutToResize(DragPosition dragPosition, QRect* rect);
    // Emitted if `SupportsResized`
    void resized(QRect rect);
    // Emitted if `SupportsStartAndEndResize`
    void endResize();
public:
    static void adjustRect(QRect& rect, DragPosition position, QSize newSize);
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
#if _WIN32
    void windowPosChanging(MSG* msg, qintptr* result);
    void windowPosChanged(MSG* msg);
#endif
#if __linux__
private:
    using DesktopNativeEventFilter = bool(ResizeEventFilter::*)(xcb_generic_event_t* event);
    bool nativeEventFilterOnKDE(xcb_generic_event_t* event);
    bool nativeEventFilterOnGNOME(xcb_generic_event_t* event);
    bool nativeEventFilterOnUnknown(xcb_generic_event_t* event);
#endif
private:
    WindowAndId windowAndId_;
    DragPosition position_ = DragPosition::Invalid;
    bool resizing_ = false;
#if _WIN32
    WPARAM ncLButtonDownMessage_ = 0;
    enum State: std::uint8_t
    {
        Exited,
        InteractiveResizeReady,
        InteractiveResizing,
        ProgrammaticResizing,
        ProgrammaticNotResizing,
        InteractiveNotResizing
    };
    State state_ = Exited;
    bool prevIsCaptureChanged_ = false;
#elif __linux__
    static DesktopNativeEventFilter desktopNativeEventFilter;
    int lastResponseType_ = XCB_GE_GENERIC + 1;
#endif
};
}

#endif // YADAW_SRC_UI_RESIZEEVENTFILTER