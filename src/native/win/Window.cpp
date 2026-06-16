#if _WIN32

#include "native/Native.hpp"
#include "native/Window.hpp"

namespace YADAW::Native
{
void showWindowWithoutActivating(QWindow& window)
{
    window.setFlag(Qt::WindowType::WindowDoesNotAcceptFocus, true);
    window.showNormal();
    window.setVisible(true);
}

QRect getPhysicalGeometry(QWindow& window)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    if(auto parent = window.parent())
    {
        auto parentGeometry = getPhysicalGeometry(*parent);
        RECT rect1; GetWindowRect(hwnd, &rect1);
        auto x = rect1.left - parentGeometry.left();
        auto y = rect1.top  - parentGeometry.top ();
        return QRect(rect1.left - parentGeometry.left(), rect1.top - parentGeometry.top(), rect1.right - rect1.left, rect1.bottom - rect1.top);
    }
    else
    {
        RECT rect1, rect2;
        GetClientRect(hwnd, &rect1);
        auto width = rect1.right;
        auto height = rect1.bottom;
        AdjustWindowRectExForDpi(&rect1, GetWindowLongPtrW(hwnd, GWL_STYLE), FALSE, GetWindowLongPtrW(hwnd, GWL_EXSTYLE), GetDpiForWindow(hwnd));
        GetWindowRect(hwnd, &rect2);
        return QRect(rect2.left - rect1.left, rect2.top - rect1.top, width, height);
    }
}

QRect getPhysicalFrameGeometry(QWindow& window)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    if(window.parent())
    {
        return getPhysicalGeometry(window);
    }
    RECT rect; GetWindowRect(hwnd, &rect);
    return QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

bool setPhysicalGeometry(QWindow& window, const QRect& physicalGeometry)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    RECT rect {
        .left = physicalGeometry.left(),
        .top = physicalGeometry.top(),
        .right = physicalGeometry.left() + physicalGeometry.width(),
        .bottom = physicalGeometry.top() + physicalGeometry.height()
    };
    auto style   = GetWindowLongPtrW(hwnd, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, GetDpiForWindow(hwnd));
    return SetWindowPos(
        hwnd, HWND_TOP/*unused*/,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

bool setPhysicalSize(QWindow& window, const QSize& physicalSize)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    RECT rect {
        .left = 0,
        .top = 0,
        .right = physicalSize.width(),
        .bottom = physicalSize.height()
    };
    auto style   = GetWindowLongPtrW(hwnd, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, GetDpiForWindow(hwnd));
    return SetWindowPos(
        hwnd, HWND_TOP/*unused*/,
        0, 0, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE
    );
}

bool setPhysicalPosition(QWindow& window, const QPoint& physicalPosition)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    RECT rect {
        .left = physicalPosition.x(),
        .top = physicalPosition.y(),
        .right = 0,
        .bottom = 0
    };
    auto style   = GetWindowLongPtrW(hwnd, GWL_STYLE);
    auto exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, GetDpiForWindow(hwnd));
    return SetWindowPos(
        hwnd, HWND_TOP/*unused*/,
        rect.left, rect.top, 0, 0,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE
    );
}

bool setPhysicalFramePosition(QWindow& window, const QPoint& physicalPosition)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    return SetWindowPos(hwnd, HWND_TOP/*unused*/,
        physicalPosition.x(), physicalPosition.y(), 0, 0,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE
    );
}

bool isWindowResizableByUser(QWindow& window)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    return GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME;
}

void setWindowResizableByUser(QWindow& window, bool resizable)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    auto style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if(resizable)
    {
        style |= WS_THICKFRAME;
        SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    }
    else if(style & WS_THICKFRAME)
    {
        style ^= WS_THICKFRAME;
        SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    }
}

bool isWindowMaximized(QWindow& window)
{
    return GetWindowLongPtrW(reinterpret_cast<HWND>(window.winId()), GWL_STYLE) & WS_MAXIMIZE;
}

void enterFullscreen(QWindow& window)
{
    if(isWindowMaximized(window))
    {
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo {
            .cbSize = sizeof(MONITORINFO)
        };
        GetMonitorInfoW(monitor, &monitorInfo);
        auto monitorRect = monitorInfo.rcMonitor;
        SetWindowLongPtrW(
            hwnd, GWL_STYLE,
            WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MAXIMIZE
        );
        SetWindowPos(
            hwnd, HWND_TOP,
            monitorRect.left, monitorRect.top,
            monitorRect.right - monitorRect.left,
            monitorRect.bottom - monitorRect.top,
            SWP_NOACTIVATE | SWP_FRAMECHANGED
        );
    }
    else
    {
        window.showFullScreen();
    }
}

void exitFullscreen(QWindow& window, bool previouslyMaximized)
{
    if(previouslyMaximized)
    {
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        constexpr auto style =
              WS_POPUP
            | WS_VISIBLE
            | WS_MAXIMIZE
            | WS_SYSMENU
            | WS_THICKFRAME
            | WS_CAPTION
            | WS_CLIPSIBLINGS
            | WS_CLIPCHILDREN
            | WS_THICKFRAME
            | WS_MAXIMIZEBOX
            | WS_MINIMIZEBOX;
        SetWindowLongPtrW(hwnd, GWL_STYLE, style);
        SetWindowPos(
            hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED
        );
    }
    else
    {
        window.showNormal();
    }
}
}

#endif