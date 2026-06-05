#include "ResizeEventFilter.hpp"
#include "ResizeEventFilter.hpp"
#include "ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMetaMethod>

#if _WIN32
#include <Windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#elif __linux__
#include <xcb/xproto.h>

#include <cinttypes>
#endif

#if _WIN32

RECT rectFromQRect(const QRect& rect)
{
    return RECT {
        .left   = rect.left(),
        .top    = rect.top (),
        .right  = rect.left() + rect.width(), // Why not `rect.right()` : https://doc.qt.io/qt-6/qrect.html#coordinates
        .bottom = rect.top () + rect.height() // Why noy 'rect.bottom()`: https://doc.qt.io/qt-6/qrect.html#coordinates
    };
}

QRect rectToQRect(const RECT& rect)
{
    // Why not `QRect::QRect(const QPoint& topLeft, const QPoint& bottomRight)`:
    // https://doc.qt.io/qt-6/qrect.html#coordinates
    return QRect(
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
    );
}

QRect windowRectFromClient(const QRect& clientRect, HWND window)
{
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    auto crect = rectFromQRect(clientRect);
    AdjustWindowRectEx(&crect, style, FALSE, exStyle);
    return rectToQRect(crect);
}

QRect clientRectFromWindow(const QRect& windowRect, HWND window)
{
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    auto wrect = rectFromQRect(windowRect);
    RECT extendedRect = wrect;
    // Inverse of `AdjustWindowRectEx`: https://stackoverflow.com/questions/11584042/inverse-of-adjustwindowrectex
    // `extendedRect` always contains `wrect`...
    AdjustWindowRectEx(&extendedRect, style, FALSE, exStyle);
    // ... but what should this return if `wrect` is too small?
    wrect.left   += wrect.left   - extendedRect.left;
    wrect.right  += wrect.right  - extendedRect.right;
    wrect.top    += wrect.top    - extendedRect.top;
    wrect.bottom += wrect.bottom - extendedRect.bottom;
    return rectToQRect(wrect);
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
    return FeatureSupportFlag::SupportsStartAndEndResize
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
        return FeatureSupportFlag::SupportsStartAndEndResize
            |  FeatureSupportFlag::SupportsAboutToResize
            |  FeatureSupportFlag::SupportsResized;
    }
    // TODO: Add status and events sent of other DEs
    return 0;
#endif
}

#if __linux__
bool ResizeEventFilter::nativeEventFilterOnKDE(xcb_generic_event_t* event)
{    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
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
    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
        &ResizeEventFilter::aboutToResize
    );
    auto responseType = event->response_type & 0x7F;
    if(responseType == XCB_CONFIGURE_NOTIFY)
    {
        if(lastResponseType_ == XCB_FOCUS_OUT)
        {
            resizing_ = true;
            startResize();
            if(auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
            {
                if(auto connection = x11Interface->connection())
                {
                    auto windowHandle = static_cast<xcb_window_t>(windowAndId_.winId);
                    auto pointerCookie = xcb_query_pointer(connection, windowHandle);
                    xcb_generic_error_t* error = nullptr;
                    auto pointerReply = xcb_query_pointer_reply(connection, pointerCookie, &error);
                    // TODO: Deduct drag position from pointerReply->win_x and win_y
                }
            }
        }
        auto configureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t*>(event);
        if(configureNotifyEvent->window == windowAndId_.winId)
        {
            QRect newGeometry(
                configureNotifyEvent->x, configureNotifyEvent->y,
                configureNotifyEvent->width, configureNotifyEvent->height
            );
            aboutToResize(ResizeEventFilter::DragPosition::BottomRight, &newGeometry);
        }
    }
    else if(resizing_)
    {
        if(responseType == XCB_FOCUS_IN)
        {
            endResize();
            resizing_ = false;
        }
        else if(responseType == XCB_EXPOSE && lastResponseType_ == XCB_CONFIGURE_NOTIFY)
        {
            resized(windowAndId_.window->geometry());
        }
    }
    if(responseType != XCB_CLIENT_MESSAGE)
    {
        lastResponseType_ = responseType;
    }
    return false;
}

bool ResizeEventFilter::nativeEventFilterOnUnknown(xcb_generic_event_t* event)
{
    auto responseType = event->response_type & 0x7F;
    if(responseType == XCB_CONFIGURE_NOTIFY)
    {
        auto configureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t*>(event);
        if(configureNotifyEvent->window == windowAndId_.winId)
        {
            QRect newGeometry(
                configureNotifyEvent->x, configureNotifyEvent->y,
                configureNotifyEvent->width, configureNotifyEvent->height
            );
            aboutToResize(ResizeEventFilter::DragPosition::BottomRight, &newGeometry);
        }
    }
    else if(responseType == XCB_EXPOSE && lastResponseType_ == XCB_CONFIGURE_NOTIFY)
    {
        resized(windowAndId_.window->geometry());
    }
    return false;
}
#endif

bool ResizeEventFilter::resizing() const
{
    return resizing_;
}


void ResizeEventFilter::adjustRect(QRect& rect, ResizeEventFilter::DragPosition position, QSize newSize)
{
    using YADAW::UI::ResizeEventFilter;
    // Why +1/-1: https://doc.qt.io/qt-6/qrect.html#coordinates
    switch(position)
    {
    case ResizeEventFilter::DragPosition::TopLeft:
        rect.setTopLeft(QPoint(rect.right() - newSize.width(), rect.bottom() - newSize.height()));
        return;
    case ResizeEventFilter::DragPosition::Top:
    case ResizeEventFilter::DragPosition::TopRight:
        rect.setTopRight(QPoint(rect.left() + newSize.width() - 1, rect.bottom() - newSize.height()));
        return;
    case ResizeEventFilter::DragPosition::Left:
    case ResizeEventFilter::DragPosition::BottomLeft:
        rect.setBottomLeft(QPoint(rect.right() - newSize.width(), rect.top() + newSize.height() - 1));
        return;
    case ResizeEventFilter::DragPosition::Right:
    case ResizeEventFilter::DragPosition::Bottom:
    case ResizeEventFilter::DragPosition::BottomRight:
        rect.setBottomRight(QPoint(rect.left() + newSize.width() - 1, rect.top() + newSize.height() - 1));
        return;
    }
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
        auto msg = static_cast<MSG*>(message);
        if(msg->hwnd != reinterpret_cast<HWND>(windowAndId_.winId))
        {
            return false;
        }
        if(msg->message == WM_NCLBUTTONDOWN
            && (msg->wParam >= HTLEFT && msg->wParam <= HTBOTTOMRIGHT))
        {
            position_ = positions[msg->wParam - HTLEFT];
            aboutToStartResize_ = true;
            ret = false;
        }
        else if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_SIZE)
        {
            // TODO: App Menu -> Size
        }
        else if(msg->message == WM_ENTERSIZEMOVE && aboutToStartResize_)
        {
            state_ = State::InteractiveResizeReady;
            aboutToStartResize_ = false;
            resizing_ = true;
            startResize();
            ret = false;
        }
        // Since all Qt functions that changes window geometry (i.e. resizes or
        // moves the window) call `QWindow::setGeometry` that calls Windows
        // function `MoveWindow` which sends `WM_WINDOWPOSCHANGING` and
        // `WM_WINDOWPOSCHANGED`, we'd better not emit `aboutToResize` and
        // `resized` when moving the window.
        else if(msg->message == WM_WINDOWPOSCHANGING)
        {
            if(state_ == State::InteractiveResizeReady && !prevIsCaptureChanged_)
            {
                state_ = State::InteractiveResizing;
                windowPosChanging(msg, result);
                *result = 0;
                ret = true;
            }
            else if(state_ == State::InteractiveResizing)
            {
                std::fprintf(stderr, "InteractiveResizing\n");
            }
            else if(state_ == State::Exited)
            {
                auto windowPos = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                if(windowPos->flags & SWP_NOSIZE)
                {
                    state_ = State::ProgrammaticNotResizing;
                    ret = false;
                }
                else
                {
                    RECT oldNativeRect; GetWindowRect(msg->hwnd, &oldNativeRect);
                    if(oldNativeRect.right - oldNativeRect.left == windowPos->cx
                        && oldNativeRect.bottom - oldNativeRect.top == windowPos->cy)
                    {
                        state_ = State::ProgrammaticNotResizing;
                        ret = false;
                    }
                    else
                    {
                        state_ = State::ProgrammaticResizing;
                        windowPosChanging(msg, result);
                        *result = 0;
                        ret = true;
                    }
                }
            }
        }
        else if(msg->message == WM_WINDOWPOSCHANGED)
        {
            if(state_ == State::InteractiveResizing)
            {
                windowPosChanged(msg);
                state_ = State::InteractiveResizeReady;
                ret = true;
            }
            else if(state_ == State::ProgrammaticResizing)
            {
                windowPosChanged(msg);
                state_ = State::Exited;
                ret = true;
            }
            else if(state_ == State::ProgrammaticNotResizing)
            {
                state_ = State::Exited;
                *result = 1;
                ret = false;
            }
            else
            {
                ret = false;
            }
        }
        else if(msg->message == WM_EXITSIZEMOVE)
        {
            position_ = DragPosition::Invalid;
            if(state_ == State::InteractiveResizeReady)
            {
                endResize();
                resizing_ = false;
                state_ = State::Exited;
                *result = 0;
                ret = true;
            }
            else
            {
                //
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

#if _WIN32
void ResizeEventFilter::windowPosChanging(MSG* msg, qintptr* result)
{
    auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
    QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
    rect = clientRectFromWindow(rect, msg->hwnd);
    aboutToResize(position_, &rect);
    rect = windowRectFromClient(rect, msg->hwnd);
    nativeRect->x = rect.left();
    nativeRect->y = rect.top();
    nativeRect->cx = rect.width();
    nativeRect->cy = rect.height();
    *result = 0;
}

void ResizeEventFilter::windowPosChanged(MSG* msg)
{
    auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
    QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
    rect = clientRectFromWindow(rect, msg->hwnd);
    auto size = rect.size();
    resized(rect);
}
#endif
}
