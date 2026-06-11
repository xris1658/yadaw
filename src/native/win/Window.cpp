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
        auto screen = window.screen();
        auto g = screen->size();
        auto dpi = GetDpiForWindow(hwnd);
        auto w = g.width() * dpi / USER_DEFAULT_SCREEN_DPI;
        auto h = g.height() * dpi / USER_DEFAULT_SCREEN_DPI;
        SetWindowLongPtrW(
            hwnd, GWL_STYLE,
            WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MAXIMIZE
        );
        SetWindowPos(
            hwnd, HWND_TOP, 0, 0, w, h, SWP_NOACTIVATE | SWP_FRAMECHANGED
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