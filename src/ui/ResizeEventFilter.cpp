#include "ResizeEventFilter.hpp"
#include "ResizeEventFilter.hpp"
#include "ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QMetaMethod>

#if _WIN32
#include <Windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#elif __linux__
#include <xcb/xproto.h>
#endif

#if _WIN32

#include <mutex>

QMargins clientMargins;

std::once_flag getMarginsFlag;

void getMargins()
{
    clientMargins.setLeft(
        GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER)
    );
    clientMargins.setRight(clientMargins.left());
    clientMargins.setBottom(
        GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER)
    );
    clientMargins.setTop(clientMargins.bottom() + GetSystemMetrics(SM_CYCAPTION));
}
#endif

namespace YADAW::UI
{
#if __linux__
    ResizeEventFilter::DesktopNativeEventFilter ResizeEventFilter::desktopNativeEventFilter;
#endif
ResizeEventFilter::ResizeEventFilter(QWindow& window):
    windowAndId_(window)
{
#if __linux__
    // Since different DEs have their own event patterns on resizing a window,
    // we have to check the pattern and interpret those events by myself
    // (e.g. by running `xev` and monitoring events on resizing the window).
    // I haven't taken Wayland into account for now. Sorry about that.
    auto desktop = std::getenv("XDG_SESSION_DESKTOP");
    if(std::strstr(desktop, "KDE"))
    {
        desktopNativeEventFilter = &ResizeEventFilter::nativeEventFilterOnKDE;
    }
    else if(std::strstr(desktop, "GNOME"))
    {
        desktopNativeEventFilter = &ResizeEventFilter::nativeEventFilterOnGNOME;
    }
    else
    {
        desktopNativeEventFilter = &ResizeEventFilter::nativeEventFilterOnUnknown;
    }
#endif
    QCoreApplication::instance()->installNativeEventFilter(this);
}

ResizeEventFilter::~ResizeEventFilter()
{
    if(resizing_)
    {
        resized(windowAndId_.window->geometry());
        endResize();
    }
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

ResizeEventFilter::FeatureSupportFlags ResizeEventFilter::getNativeSupportFlags()
{
#if _WIN32
    return FeatureSupportFlag::SupportsStartStopResize
        |  FeatureSupportFlag::SupportsAboutToResize
        |  FeatureSupportFlag::SupportsDragPosition
        |  FeatureSupportFlag::SupportsResized
        |  FeatureSupportFlag::SupportsAdjustOnAboutToResize;
#elif __linux__
    // On KDE:
    // - No events are sent on starting/ending resizing.
    // - Two `XCB_CONFIGURE_NOTIFY` events are sent while resizing a
    //   window. Both events store the new window size.
    //   `QWindow::geometry()` returns the old size on receiving the first one.
    //   (This might change if Qt updates their implementations.)
    //
    // `ResizeEventFilter`:
    // - Emits `aboutToResize()` on receiving the first `XCB_CONFIGURE_NOTIFY`
    //   event;
    // - Emits `resized()` on receiving the second `XCB_CONFIGURE_NOTIFY`.
    if(desktopNativeEventFilter == &ResizeEventFilter::nativeEventFilterOnKDE)
    {
        return FeatureSupportFlag::SupportsAboutToResize
            |  FeatureSupportFlag::SupportsResized;
    }
    // On GNOME:
    // - An `XCB_FOCUS_OUT` event is sent on starting resizing, and an
    //   `XCB_FOCUS_IN` event on ending resizing. The first event is sent
    //   whether the window has focus or not on starting resizing.
    // - Only one `XCB_CONFIGURE_NOTIFY` event is sent during resizing, and
    //   `QWindow::geometry()` returns the old size on receiving this event.
    //   (This might change if Qt updates their implementations.)
    //
    // `ResizeEventFilter`:
    // - Emits `startResizing()` on receiving the first `XCB_CONFIGURE_NOTIFY`
    //   event after the `XCB_FOCUS_OUT` event;
    // - Emits `aboutToResize()` on receiving the event. Of course,
    //   `aboutToResize()` is emitted AFTER `startResizing()`.
    // - Emits `stopResizing()` on receiving the `XCB_FOCUS_IN` event;
    if(desktopNativeEventFilter == &ResizeEventFilter::nativeEventFilterOnGNOME)
    {
        return 0;
    }
    // TODO: Add status and events sent of other DEs
    return 0;
#endif
}

#if __linux__
bool ResizeEventFilter::nativeEventFilterOnKDE(xcb_generic_event_t* event)
{
    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
        &ResizeEventFilter::aboutToResize
    );
    auto responseType = event->response_type & 0x7F;
    if(responseType == XCB_CONFIGURE_NOTIFY)
    {
        auto configureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t*>(event);
        if(configureNotifyEvent->window == windowAndId_.winId)
        {
            if(auto geometry = windowAndId_.window->geometry();
                configureNotifyEvent->width != geometry.width()
                || configureNotifyEvent->height != geometry.height()
            )
            {
                QRect newGeometry(
                    configureNotifyEvent->x, configureNotifyEvent->y,
                    configureNotifyEvent->width, configureNotifyEvent->height
                );
                aboutToResize(ResizeEventFilter::DragPosition::BottomRight, &newGeometry);
            }
            else
            {
                resized(geometry);
            }
        }
    }
    return false;
}

bool ResizeEventFilter::nativeEventFilterOnGNOME(xcb_generic_event_t* event)
{
    // TODO
    return false;
}

bool ResizeEventFilter::nativeEventFilterOnUnknown(xcb_generic_event_t* event)
{
    // TODO
    return false;
}
#endif

bool ResizeEventFilter::resizing() const
{
    return resizing_;
}

bool ResizeEventFilter::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
#if _WIN32
    constexpr DragPosition positions[] = {
        DragPosition::Left,
        DragPosition::Right,
        DragPosition::Top,
        DragPosition::TopLeft,
        DragPosition::TopRight,
        DragPosition::Bottom,
        DragPosition::BottomLeft,
        DragPosition::BottomRight
    };
    if(eventType == "windows_generic_MSG")
    {
        bool ret = false;
        static auto aboutToResizeSignal = QMetaMethod::fromSignal(
            &ResizeEventFilter::aboutToResize
        );
        auto msg = static_cast<MSG*>(message);
        if(msg->hwnd != reinterpret_cast<HWND>(windowAndId_.winId))
        {
            ret = false;
        }
        else if(msg->message == WM_NCLBUTTONDOWN
            && (msg->wParam >= HTLEFT && msg->wParam <= HTBOTTOMRIGHT))
        {
            position_ = positions[msg->wParam - HTLEFT];
            aboutToStartResize_ = true;
        }
        else if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_SIZE)
        {
            // TODO: App Menu -> Size
        }
        else if(msg->message == WM_ENTERSIZEMOVE && aboutToStartResize_)
        {
            aboutToStartResize_ = false;
            resizing_ = true;
            startResize();
        }
        else if(msg->message == WM_WINDOWPOSCHANGING && resizing_ && !prevIsCaptureChanged_)
        {
            if(isSignalConnected(aboutToResizeSignal))
            {
                std::call_once(getMarginsFlag, &getMargins);
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                rect = rect.marginsRemoved(clientMargins);
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(clientMargins);
                nativeRect->x = rect.left();
                nativeRect->y = rect.top();
                nativeRect->cx = rect.width();
                nativeRect->cy = rect.height();
                *result = 0;
                ret = true;
            }
        }
        else if(msg->message == WM_WINDOWPOSCHANGED && resizing_)
        {
            static auto resizedSignal = QMetaMethod::fromSignal(
                &ResizeEventFilter::resized
            );
            if(isSignalConnected(resizedSignal))
            {
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                rect = rect.marginsRemoved(clientMargins);
                resized(rect);
            }
        }
        else if(msg->message == WM_EXITSIZEMOVE && resizing_)
        {
            endResize();
            resizing_ = false;
        }
        else if(msg->message == WM_SIZING && resizing_) // Only if the resize is initiated
        {
            if(isSignalConnected(aboutToResizeSignal))
            {
                std::call_once(getMarginsFlag, &getMargins);
                position_ = positions[msg->wParam - WMSZ_LEFT];
                auto nativeRect = reinterpret_cast<RECT*>(msg->lParam);
                QRect rect(nativeRect->left, nativeRect->top,
                    nativeRect->right - nativeRect->left, nativeRect->bottom - nativeRect->top
                );
                rect = rect.marginsRemoved(clientMargins);
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(clientMargins);
                nativeRect->left = rect.left();
                nativeRect->top = rect.top();
                nativeRect->right = rect.width();
                nativeRect->bottom = rect.height();
                *result = 0;
                ret = true;
            }
        }
        prevIsCaptureChanged_ = msg->message == WM_CAPTURECHANGED;
        return ret;
    }
#elif __linux__
    if(eventType == "xcb_generic_event_t")
    {
        auto event = static_cast<xcb_generic_event_t*>(message);
        auto func = desktopNativeEventFilter;
        (this->*func)(event);
    }
#endif
    return false;
}

void adjustRect(QRect& rect, ResizeEventFilter::DragPosition position, QSize newSize)
{
    using YADAW::UI::ResizeEventFilter;
    // See `QRect::right()` and `QRect::bottom()`
    if(position == ResizeEventFilter::DragPosition::TopLeft)
    {
        rect.setTopLeft(QPoint(rect.right() - newSize.width(), rect.bottom() - newSize.height()));
    }
    else if(position == ResizeEventFilter::DragPosition::Top || position == ResizeEventFilter::DragPosition::TopRight)
    {
        rect.setTopRight(QPoint(rect.left() + newSize.width() - 1, rect.bottom() - newSize.height()));
    }
    else if(position == ResizeEventFilter::DragPosition::Left || position == ResizeEventFilter::DragPosition::BottomLeft)
    {
        rect.setBottomLeft(QPoint(rect.right() - newSize.width(), rect.top() + newSize.height() - 1));
    }
    else
    {
        rect.setBottomRight(QPoint(rect.left() + newSize.width() - 1, rect.top() + newSize.height() - 1));
    }
}
}
