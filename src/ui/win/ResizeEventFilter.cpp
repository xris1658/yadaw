#if _WIN32

#include "ui/ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMetaMethod>

#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

// Macros that control the debug messages, uncomment these on demand
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES 1
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES   1

#if YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES
std::map<UINT, const char*> messages(winMessages);

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

QRect windowRectFromClient(const QRect& clientRect, HWND window, UINT dpi)
{
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    auto crect = rectFromQRect(clientRect);
    AdjustWindowRectExForDpi(&crect, style, FALSE, exStyle, dpi);
    return rectToQRect(crect);
}

QRect clientRectFromWindow(const QRect& windowRect, HWND window, UINT dpi)
{
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    auto wrect = rectFromQRect(windowRect);
    RECT extendedRect = wrect;
    // Inverse of `AdjustWindowRectEx`: https://stackoverflow.com/questions/11584042/inverse-of-adjustwindowrectex
    // `extendedRect` always contains `wrect`...
    AdjustWindowRectExForDpi(&extendedRect, style, FALSE, exStyle, dpi);
    // ... but what should this return if `wrect` is too small?
    wrect.left   += wrect.left   - extendedRect.left;
    wrect.right  += wrect.right  - extendedRect.right;
    wrect.top    += wrect.top    - extendedRect.top;
    wrect.bottom += wrect.bottom - extendedRect.bottom;
    return rectToQRect(wrect);
}

QRect diRectFromPhysicalRect(const QRect& physicalRect, UINT dpi)
{
    int rect[4]; auto& [x, y, w, h] = rect;
    physicalRect.getRect(&x, &y, &w, &h);
    x *= USER_DEFAULT_SCREEN_DPI; x /= dpi;
    y *= USER_DEFAULT_SCREEN_DPI; y /= dpi;
    w *= USER_DEFAULT_SCREEN_DPI; w /= dpi;
    h *= USER_DEFAULT_SCREEN_DPI; h /= dpi;
    return QRect(x, y, w, h);
}

QRect diRectToPhysicalRect(const QRect& diRect, UINT dpi)
{
    int rect[4]; auto& [x, y, w, h] = rect;
    diRect.getRect(&x, &y, &w, &h);
    x *= dpi; x /= USER_DEFAULT_SCREEN_DPI;
    y *= dpi; y /= USER_DEFAULT_SCREEN_DPI;
    w *= dpi; w /= USER_DEFAULT_SCREEN_DPI;
    h *= dpi; h /= USER_DEFAULT_SCREEN_DPI;
    return QRect(x, y, w, h);
}

namespace YADAW::UI
{
ResizeEventFilter::ResizeEventFilter(QWindow& window):
    windowAndId_(window)
{
    QCoreApplication::instance()->installNativeEventFilter(this);
}

ResizeEventFilter::FeatureSupportFlags ResizeEventFilter::getNativeSupportFlags()
{
    return FeatureSupportFlag::SupportsStartAndEndResize
         | FeatureSupportFlag::SupportsAboutToResize
         | FeatureSupportFlag::SupportsDragPosition
         | FeatureSupportFlag::SupportsResized
         | FeatureSupportFlag::SupportsAdjustOnAboutToResize
         | FeatureSupportFlag::UsesPhysicalSize;
}

bool ResizeEventFilter::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES
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
        if(msg->message == WM_NCLBUTTONDOWN)
        {
            ncLButtonDownMessage_ = msg->wParam;
        }
        else if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_SIZE)
        {
            // TODO: App Menu -> Size
        }
        else if(msg->message == WM_ENTERSIZEMOVE)
        {
            if(ncLButtonDownMessage_ >= HTLEFT && ncLButtonDownMessage_ <= HTBOTTOMRIGHT)
            {
                position_ = positions[ncLButtonDownMessage_ - HTLEFT];
                state_ = State::InteractiveResizeReady;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to InteractiveResizeReady on pressing the window border\n");
#endif
                resizing_ = true;
                startResize();
                ret = false;
            }
            else
            {
                state_ = State::InteractiveNotResizing;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to InteractiveNotResizing\n");
#endif
            }
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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to InteractiveResizing\n");
#endif
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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                    std::fprintf(stderr, "[DEBUG] State moved to ProgrammaticNotResizing, seems like `MoveWindow` called but `SWP_NORESIZE` is set\n");
#endif
                    ret = false;
                }
                else
                {
                    RECT oldNativeRect; GetWindowRect(msg->hwnd, &oldNativeRect);
                    if(oldNativeRect.right - oldNativeRect.left == windowPos->cx
                        && oldNativeRect.bottom - oldNativeRect.top == windowPos->cy)
                    {
                        state_ = State::ProgrammaticNotResizing;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                        std::fprintf(stderr, "[DEBUG] State moved to ProgrammaticNotResizing, seems like `MoveWindow` called but window size has not changed\n");
#endif
                        ret = false;
                    }
                    else
                    {
                        state_ = State::ProgrammaticResizing;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                        std::fprintf(stderr, "[DEBUG] State moved to ProgrammaticResizing\n");
#endif
                        windowPosChanging(msg, result);
                        *result = 0;
                        ret = true;
                    }
                }
            }
            else if(state_ == State::InteractiveNotResizing)
            {
                ret = false;
            }
        }
        else if(msg->message == WM_WINDOWPOSCHANGED)
        {
            if(state_ == State::InteractiveResizing)
            {
                windowPosChanged(msg);
                state_ = State::InteractiveResizeReady;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to InteractiveResizeReady on `WM_WINDOWPOSCHANGED`\n");
#endif
                ret = true;
            }
            else if(state_ == State::ProgrammaticResizing)
            {
                windowPosChanged(msg);
                state_ = State::Exited;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to Exited from ProgrammaticResizing\n");
#endif
                ret = false;
            }
            else if(state_ == State::ProgrammaticNotResizing)
            {
                state_ = State::Exited;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to Exited from ProgrammaticNotResizing\n");
#endif
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
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to Exited from InteractiveResizeReady\n");
#endif
                *result = 0;
                ret = true;
            }
            else if(state_ == State::InteractiveNotResizing)
            {
                state_ = State::Exited;
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES
                std::fprintf(stderr, "[DEBUG] State moved to Exited from InteractiveNotResizing\n");
#endif
            }
        }
        prevIsCaptureChanged_ = msg->message == WM_CAPTURECHANGED;

#if YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES
        if(msg->message == WM_WINDOWPOSCHANGING)
        {
            printWindowPos(msg);
        }
#endif
        return ret;
    }
    return false;
}

void ResizeEventFilter::windowPosChanging(MSG* msg, qintptr* result)
{
    auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
    QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
    auto dpi = GetDpiForWindow(msg->hwnd);
    rect = clientRectFromWindow(rect, msg->hwnd, dpi);
    aboutToResize(position_, &rect);
    rect = windowRectFromClient(rect, msg->hwnd, dpi);
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
    auto dpi = GetDpiForWindow(msg->hwnd);
    rect = clientRectFromWindow(rect, msg->hwnd, dpi);
    resized(rect);
}
}
#if YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES
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
            std::fprintf(stderr, "%s%s", leftParen? " | ": " (", swp[i]);
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