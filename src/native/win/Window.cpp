#if _WIN32

#include "native/Library.hpp"
#include "native/Window.hpp"

#include <mutex>

namespace YADAW::Native
{
void showWindowWithoutActivating(QWindow& window)
{
    window.setFlag(Qt::WindowType::WindowDoesNotAcceptFocus, true);
    window.showNormal();
    window.setVisible(true);
}

bool isWindows11 = false;

std::once_flag getWindowsVersionFlag;

void getWindowsIs11()
{
    auto build = YADAW::Native::getWindowsVersion().buildVersion;
    isWindows11 = (build >= 21996);
}

bool isWindowResizable(QWindow& window)
{
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    return GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME;
}

void setWindowResizable(QWindow& window, bool resizable)
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

void enterFullscreen(QWindow& window)
{
    if(window.visibility() == QWindow::Visibility::Maximized)
    {
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        const UINT swpf = SWP_FRAMECHANGED | SWP_NOACTIVATE;
        auto oldGeometry = window.geometry();
        // We only pass the new sizing and positioning flags to the window, without
        // actually setting the window geometry.
        SetWindowPos(hwnd, HWND_TOP,
            oldGeometry.left(), oldGeometry.top(),
            oldGeometry.width(), oldGeometry.height(),
            swpf
        );
        // This `setGeometry` is necessary to make `exitFullscreen`
        // work without the intermediate normal visibility.
        window.setGeometry(oldGeometry);
        window.showFullScreen();
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
        std::call_once(getWindowsVersionFlag, &getWindowsIs11);
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        // We only pass the new sizing and positioning flags to the window, without
        // actually setting the window geometry.
        if(isWindows11)
        {
            window.showMinimized();
        }
        window.showMaximized();
    }
    else
    {
        window.showNormal();
    }
}
}

#endif