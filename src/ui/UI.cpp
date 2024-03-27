#include "UI.hpp"

#include "native/Native.hpp"

#include <QCoreApplication>

#if __linux__
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

namespace YADAW::UI
{
QQmlApplicationEngine* qmlApplicationEngine = nullptr;
QQuickWindow* mainWindow = nullptr;

const QString& defaultFontDir()
{
    static QString ret =
        QCoreApplication::applicationDirPath()
        + YADAW::Native::PathSeparator + "content"
        + YADAW::Native::PathSeparator + "fonts";
    return ret;
}

void setMaximizedWindowToFullScreen(QWindow& window)
{
    auto fallback = true;
#if _WIN32
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
    // This `setGeometry` is necessary to make `setFullScreenWindowToMaximized`
    // work without the intermediate normal visibility.
    window.setGeometry(oldGeometry);
    window.showFullScreen();
    fallback = false;
#endif
    if(fallback)
    {
        window.showFullScreen();
    }
}

void setFullScreenWindowToMaximized(QWindow& window)
{
    auto fallback = true;
#if _WIN32
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    UINT swpf = SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    auto oldGeometry = window.geometry();
    // We only pass the new sizing and positioning flags to the window, without
    // actually setting the window geometry.
    SetWindowPos(hwnd, nullptr, oldGeometry.left(), oldGeometry.top(),
        oldGeometry.width(), oldGeometry.height(),
        swpf
    );
    window.showMaximized();
    fallback = false;
#endif
    if(fallback)
    {
        window.showMaximized();
    }
}
}
