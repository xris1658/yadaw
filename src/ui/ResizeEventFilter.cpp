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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER
std::map<UINT, const char*> messages {
    {WM_NULL, "WM_NULL"},
    {WM_CREATE, "WM_CREATE"},
    {WM_DESTROY, "WM_DESTROY"},
    {WM_MOVE, "WM_MOVE"},
    {WM_SIZE, "WM_SIZE"},
    {WM_ACTIVATE, "WM_ACTIVATE"},
    {WM_SETFOCUS, "WM_SETFOCUS"},
    {WM_KILLFOCUS, "WM_KILLFOCUS"},
    {WM_CLOSE, "WM_CLOSE"},
    {WM_QUERYENDSESSION, "WM_QUERYENDSESSION"},
    {WM_QUIT, "WM_QUIT"},
    {WM_QUERYOPEN, "WM_QUERYOPEN"},
    {WM_ERASEBKGND, "WM_ERASEBKGND"},
    {WM_SYSCOLORCHANGE, "WM_SYSCOLORCHANGE"},
    {WM_ENDSESSION, "WM_ENDSESSION"},
    {WM_SHOWWINDOW, "WM_SHOWWINDOW"},
    {WM_SETCURSOR, "WM_SETCURSOR"},
    {WM_MOUSEACTIVATE, "WM_MOUSEACTIVATE"},
    {WM_CHILDACTIVATE, "WM_CHILDACTIVATE"},
    {WM_QUEUESYNC, "WM_QUEUESYNC"},
    {WM_GETMINMAXINFO, "WM_GETMINMAXINFO"},
    {WM_PAINTICON, "WM_PAINTICON"},
    {WM_ICONERASEBKGND, "WM_ICONERASEBKGND"},
    {WM_NEXTDLGCTL, "WM_NEXTDLGCTL"},
    {WM_SETFONT, "WM_SETFONT"},
    {WM_GETFONT, "WM_GETFONT"},
    {WM_SETHOTKEY, "WM_SETHOTKEY"},
    {WM_GETHOTKEY, "WM_GETHOTKEY"},
    {WM_QUERYDRAGICON, "WM_QUERYDRAGICON"},
    {WM_COMPAREITEM, "WM_COMPAREITEM"},
    {WM_COMPACTING, "WM_COMPACTING"},
    {WM_COMMNOTIFY, "WM_COMMNOTIFY"},
    {WM_WINDOWPOSCHANGING, "WM_WINDOWPOSCHANGING"},
    {WM_WINDOWPOSCHANGED, "WM_WINDOWPOSCHANGED"},
    {WM_POWER, "WM_POWER"},
    {WM_INPUTLANGCHANGEREQUEST, "WM_INPUTLANGCHANGEREQUEST"},
    {WM_INPUTLANGCHANGE, "WM_INPUTLANGCHANGE"},
    {WM_TCARD, "WM_TCARD"},
    {WM_HELP, "WM_HELP"},
    {WM_USERCHANGED, "WM_USERCHANGED"},
    {WM_NOTIFYFORMAT, "WM_NOTIFYFORMAT"},
    {WM_SETICON, "WM_SETICON"},
    {WM_NCCREATE, "WM_NCCREATE"},
    {WM_NCDESTROY, "WM_NCDESTROY"},
    {WM_NCCALCSIZE, "WM_NCCALCSIZE"},
    {WM_NCHITTEST, "WM_NCHITTEST"},
    {WM_NCPAINT, "WM_NCPAINT"},
    {WM_NCACTIVATE, "WM_NCACTIVATE"},
    {WM_GETDLGCODE, "WM_GETDLGCODE"},
    {WM_SYNCPAINT, "WM_SYNCPAINT"},
    {WM_KEYDOWN, "WM_KEYDOWN"},
    {WM_KEYFIRST, "WM_KEYFIRST"},
    {WM_KEYUP, "WM_KEYUP"},
    {WM_CHAR, "WM_CHAR"},
    {WM_DEADCHAR, "WM_DEADCHAR"},
    {WM_SYSKEYDOWN, "WM_SYSKEYDOWN"},
    {WM_SYSKEYUP, "WM_SYSKEYUP"},
    {WM_SYSCHAR, "WM_SYSCHAR"},
    {WM_SYSDEADCHAR, "WM_SYSDEADCHAR"},
    {WM_KEYLAST, "WM_KEYLAST"},
    {WM_UNICHAR, "WM_UNICHAR"},
    {WM_INITDIALOG, "WM_INITDIALOG"},
    {WM_COMMAND, "WM_COMMAND"},
    {WM_SYSCOMMAND, "WM_SYSCOMMAND"},
    {WM_TIMER, "WM_TIMER"},
    {WM_HSCROLL, "WM_HSCROLL"},
    {WM_VSCROLL, "WM_VSCROLL"},
    {WM_INITMENU, "WM_INITMENU"},
    {WM_INITMENUPOPUP, "WM_INITMENUPOPUP"},
    {WM_GESTURE, "WM_GESTURE"},
    {WM_MENUCHAR, "WM_MENUCHAR"},
    {WM_ENTERIDLE, "WM_ENTERIDLE"},
    {WM_MENURBUTTONUP, "WM_MENURBUTTONUP"},
    {WM_MENUDRAG, "WM_MENUDRAG"},
    {WM_MENUGETOBJECT, "WM_MENUGETOBJECT"},
    {WM_UNINITMENUPOPUP, "WM_UNINITMENUPOPUP"},
    {WM_MENUCOMMAND, "WM_MENUCOMMAND"},
    {WM_CHANGEUISTATE, "WM_CHANGEUISTATE"},
    {WM_UPDATEUISTATE, "WM_UPDATEUISTATE"},
    {WM_QUERYUISTATE, "WM_QUERYUISTATE"},
    {WM_CTLCOLORMSGBOX, "WM_CTLCOLORMSGBOX"},
    {WM_CTLCOLOREDIT, "WM_CTLCOLOREDIT"},
    {WM_CTLCOLORLISTBOX, "WM_CTLCOLORLISTBOX"},
    {WM_CTLCOLORBTN, "WM_CTLCOLORBTN"},
    {WM_CTLCOLORDLG, "WM_CTLCOLORDLG"},
    {WM_CTLCOLORSCROLLBAR, "WM_CTLCOLORSCROLLBAR"},
    {WM_CTLCOLORSTATIC, "WM_CTLCOLORSTATIC"},
    {WM_MOUSEFIRST, "WM_MOUSEFIRST"},
    {WM_MOUSEMOVE, "WM_MOUSEMOVE"},
    {WM_LBUTTONDOWN, "WM_LBUTTONDOWN"},
    {WM_LBUTTONUP, "WM_LBUTTONUP"},
    {WM_LBUTTONDBLCLK, "WM_LBUTTONDBLCLK"},
    {WM_RBUTTONDOWN, "WM_RBUTTONDOWN"},
    {WM_RBUTTONUP, "WM_RBUTTONUP"},
    {WM_RBUTTONDBLCLK, "WM_RBUTTONDBLCLK"},
    {WM_MBUTTONDOWN, "WM_MBUTTONDOWN"},
    {WM_MBUTTONUP, "WM_MBUTTONUP"},
    {WM_MBUTTONDBLCLK, "WM_MBUTTONDBLCLK"},
    {WM_MOUSELAST, "WM_MOUSELAST"},
    {WM_PARENTNOTIFY, "WM_PARENTNOTIFY"},
    {WM_ENTERMENULOOP, "WM_ENTERMENULOOP"},
    {WM_EXITMENULOOP, "WM_EXITMENULOOP"},
    {WM_NEXTMENU, "WM_NEXTMENU"},
    {WM_SIZING, "WM_SIZING"},
    {WM_CAPTURECHANGED, "WM_CAPTURECHANGED"},
    {WM_MOVING, "WM_MOVING"},
    {WM_POWERBROADCAST, "WM_POWERBROADCAST"},
    {WM_DEVICECHANGE, "WM_DEVICECHANGE"},
    {WM_MDICREATE, "WM_MDICREATE"},
    {WM_MDIDESTROY, "WM_MDIDESTROY"},
    {WM_MDIACTIVATE, "WM_MDIACTIVATE"},
    {WM_MDIRESTORE, "WM_MDIRESTORE"},
    {WM_MDINEXT, "WM_MDINEXT"},
    {WM_MDIMAXIMIZE, "WM_MDIMAXIMIZE"},
    {WM_MDITILE, "WM_MDITILE"},
    {WM_MDICASCADE, "WM_MDICASCADE"},
    {WM_MDIICONARRANGE, "WM_MDIICONARRANGE"},
    {WM_MDIGETACTIVE, "WM_MDIGETACTIVE"},
    {WM_MDISETMENU, "WM_MDISETMENU"},
    {WM_ENTERSIZEMOVE, "WM_ENTERSIZEMOVE"},
    {WM_EXITSIZEMOVE, "WM_EXITSIZEMOVE"},
    {WM_DROPFILES, "WM_DROPFILES"},
    {WM_MDIREFRESHMENU, "WM_MDIREFRESHMENU"},
    {WM_TOUCH, "WM_TOUCH"},
    {WM_NCPOINTERUPDATE, "WM_NCPOINTERUPDATE"},
    {WM_NCPOINTERDOWN, "WM_NCPOINTERDOWN"},
    {WM_NCPOINTERUP, "WM_NCPOINTERUP"},
    {WM_POINTERUPDATE, "WM_POINTERUPDATE"},
    {WM_POINTERDOWN, "WM_POINTERDOWN"},
    {WM_POINTERUP, "WM_POINTERUP"},
    {WM_POINTERENTER, "WM_POINTERENTER"},
    {WM_POINTERROUTEDTO, "WM_POINTERROUTEDTO"},
    {WM_POINTERROUTEDAWAY, "WM_POINTERROUTEDAWAY"},
    {WM_POINTERROUTEDRELEASED, "WM_POINTERROUTEDRELEASED"},
    {WM_CUT, "WM_CUT"},
    {WM_COPY, "WM_COPY"},
    {WM_PASTE, "WM_PASTE"},
    {WM_CLEAR, "WM_CLEAR"},
    {WM_UNDO, "WM_UNDO"},
    {WM_RENDERFORMAT, "WM_RENDERFORMAT"},
    {WM_RENDERALLFORMATS, "WM_RENDERALLFORMATS"},
    {WM_DESTROYCLIPBOARD, "WM_DESTROYCLIPBOARD"},
    {WM_DRAWCLIPBOARD, "WM_DRAWCLIPBOARD"},
    {WM_PAINTCLIPBOARD, "WM_PAINTCLIPBOARD"},
    {WM_PALETTEISCHANGING, "WM_PALETTEISCHANGING"},
    {WM_PALETTECHANGED, "WM_PALETTECHANGED"},
    {WM_HOTKEY, "WM_HOTKEY"},
    {WM_PRINT, "WM_PRINT"},
    {WM_PRINTCLIENT, "WM_PRINTCLIENT"},
    {WM_APPCOMMAND, "WM_APPCOMMAND"},
    {WM_DWMCOLORIZATIONCOLORCHANGED, "WM_DWMCOLORIZATIONCOLORCHANGED"},
    {WM_DWMWINDOWMAXIMIZEDCHANGE, "WM_DWMWINDOWMAXIMIZEDCHANGE"},
    {WM_DWMSENDICONICTHUMBNAIL, "WM_DWMSENDICONICTHUMBNAIL"},
    {WM_DWMSENDICONICLIVEPREVIEWBITMAP, "WM_DWMSENDICONICLIVEPREVIEWBITMAP"},
    {WM_HANDHELDFIRST, "WM_HANDHELDFIRST"},
    {WM_AFXFIRST, "WM_AFXFIRST"},
    {WM_PENWINFIRST, "WM_PENWINFIRST"},
    {WM_USER, "WM_USER"}
};

void printWindowPos(MSG* msg);

void printSize(MSG* msg);

void printSizing(MSG* msg);
#endif
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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER
        auto it = messages.find(LOWORD(msg->message));
        if(it != messages.end())
        {
            if(it->first == WM_WINDOWPOSCHANGING || it->first == WM_WINDOWPOSCHANGED)
            {
                std::fprintf(stderr, "[DEBUG] %s\n", it->second);
                printWindowPos(msg);
            }
            else if(it->first == WM_SIZE)
            {
                std::fprintf(stderr, "[DEBUG] %s\n", it->second);
                printSize(msg);
            }
            else if(it->first == WM_SIZING)
            {
                std::fprintf(stderr, "[DEBUG] %s\n", it->second);
                printSizing(msg);
            }
        }
        else
        {
            std::fprintf(stderr, "[DEBUG] 0x%04X\n", LOWORD(msg->message));
        }
#endif
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
            if((state_ == State::InteractiveResizeReady || state_ == State::InteractiveResizing) && !prevIsCaptureChanged_)
            {
                state_ = State::InteractiveResizing;
                windowPosChanging(msg, result);
                *result = 0;
                ret = true;
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
                ret = false;
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

#if YADAW_DEBUG_RESIZE_EVENT_FILTER
        if(msg->message == WM_WINDOWPOSCHANGING)
        {
            printWindowPos(msg);
        }
#endif
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
#if _WIN32
#if YADAW_DEBUG_RESIZE_EVENT_FILTER
constexpr const char* swp[] = {
    "SWP_NOSIZE",
    "SWP_NOMOVE",
    "SWP_NOZORDER",
    "SWP_NOREDRAW",
    "SWP_NOACTIVATE",
    "SWP_FRAMECHANGED",
    "SWP_SHOWWINDOW",
    "SWP_HIDEWINDOW",
    "SWP_NOCOPYBITS",
    "SWP_NOOWNERZORDER",
    "SWP_NOSENDCHANGING"
};

constexpr const char* sizeNames[] = {
    "SIZE_RESTORED",
    "SIZE_MINIMIZED",
    "SIZE_MAXIMIZED",
    "SIZE_MAXSHOW",
    "SIZE_MAXHIDE"
};

constexpr const char* edges[] = {
    "", "<-", "->", "^", "<^", "^>", "v", "<v", ">v"
};

void printWindowPos(MSG* msg)
{
    auto windowPos = reinterpret_cast<WINDOWPOS*>(msg->lParam);
    std::fprintf(stderr, "[DEBUG]   Geometry: (%d, %d), %d x %d\n", windowPos->x, windowPos->y, windowPos->cx, windowPos->cy);
    std::fprintf(stderr, "[DEBUG]     Flags: 0x%04X", windowPos->flags);
    bool leftParen = false;
    for(int i = 0; i < 11; ++i)
    {
        if(windowPos->flags & (1 << i))
        {
            std::fprintf(stderr, "%s%s", leftParen? " | ": "(", swp[i]);
            leftParen = true;
        }
    }
    std::fprintf(stderr, ")\n");
}

void printSize(MSG* msg)
{
    std::fprintf(stderr, "[DEBUG]   Type: %s\n", sizeNames[msg->wParam]);
    std::fprintf(stderr, "[DEBUG]   Size: %d x %d\n", LOWORD(msg->lParam), HIWORD(msg->lParam));
}

void printSizing(MSG* msg)
{
    std::fprintf(stderr, "[DEBUG]   Edge: %s\n", edges[msg->wParam]);
    auto rect = reinterpret_cast<RECT*>(msg->lParam);
    std::fprintf(stderr, "[DEBUG]   Geometry: (%ld, %ld), %ld x %ld\n", rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);
}
#endif
#endif
