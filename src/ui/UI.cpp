#include "UI.hpp"

#include "native/Native.hpp"
#if __APPLE__
#include "ui/mac/WindowFunctions.hpp"
#endif

#include <QCoreApplication>
#include <QGuiApplication>
#include <QWindow>

#if _WIN32
#include "native/Library.hpp"

#include <stdexcept>
#elif __linux__
#include <X11/Xutil.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>
#endif

#include <mutex>

namespace YADAW::UI
{
const QString& defaultFontDir()
{
#if __APPLE__
    static QString ret = QCoreApplication::applicationDirPath() + "/../Resources/qml/content/fonts";
#else
    static QString ret =
        QCoreApplication::applicationDirPath()
        + YADAW::Native::PathSeparator + "content"
        + YADAW::Native::PathSeparator + "fonts";
#endif
    return ret;
}

// This function exists even though there is a flag called
// `Qt::WindowDoesNotAcceptFocus`.
// On X11 with only flags set, if a native popup is shown by clicking
// outside the mouse (e.g. clicking a menu bar item that shows a menu in its
// native popup), then clicking inside the popup generates a `WM_TAKE_FOCUS`
// event, which closes the popup and leaves the click event not handled.
void showWindowWithoutActivating(QWindow& window)
{
#if __linux__
    auto winId = window.winId();
    auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if(x11Interface)
    {
        auto windowHandle = static_cast<xcb_window_t>(winId);
        auto connection = x11Interface->connection();
        auto value = 1;
        xcb_change_window_attributes(connection, windowHandle, XCB_CW_OVERRIDE_REDIRECT, &value);
        xcb_map_window(connection, windowHandle);
        xcb_flush(connection);
    }
#else
    window.setFlag(Qt::WindowType::WindowDoesNotAcceptFocus, true);
    window.showNormal();
#endif
    window.setVisible(true);
}

#if __linux__
xcb_atom_t stateAtom;
xcb_atom_t fullscreenAtom;
xcb_atom_t fillWidthAtom;
xcb_atom_t fillHeightAtom;
const char stateText[] = "_NET_WM_STATE";
const char fullscreenText[] = "_NET_WM_STATE_FULLSCREEN";
const char fillWidthText[] = "_NET_WM_STATE_MAXIMIZED_HORZ";
const char fillHeightText[] = "_NET_WM_STATE_MAXIMIZED_VERT";
const char resizeText[] = "_NET_WM_ACTION_RESIZE";
const char changeStateText[] = "WM_CHANGE_STATE";

std::once_flag initializeAtomFlag;

void initializeAtoms(xcb_connection_t* connection)
{
    xcb_intern_atom_reply_t* reply = nullptr;
    auto stateCookie = xcb_intern_atom(connection, false,
        strlen(stateText), stateText);
    reply = xcb_intern_atom_reply(connection, stateCookie, nullptr);
    stateAtom = reply->atom;
    free(reply);
    auto fullscreenCookie = xcb_intern_atom(connection, false,
        strlen(fullscreenText), fullscreenText);
    reply = xcb_intern_atom_reply(connection, fullscreenCookie, nullptr);
    fullscreenAtom = reply->atom;
    free(reply);
    auto fillWidthCookie = xcb_intern_atom(connection, false,
        strlen(fillWidthText), fillWidthText);
    reply = xcb_intern_atom_reply(connection, fillWidthCookie, nullptr);
    fillWidthAtom = reply->atom;
    free(reply);
    auto fillHeightCookie = xcb_intern_atom(connection, false,
        strlen(fillHeightText), fillHeightText);
    reply = xcb_intern_atom_reply(connection, fillHeightCookie, nullptr);
    fillHeightAtom = reply->atom;
    free(reply);
}

xcb_screen_t* getScreenOfWindow(xcb_window_t window, xcb_connection_t* connection)
{
    xcb_screen_t* ret = nullptr;
    auto geometryCookie = xcb_get_geometry_unchecked(connection, window);
    auto geometryReply = xcb_get_geometry_reply(connection, geometryCookie, nullptr);
    for(auto it = xcb_setup_roots_iterator(xcb_get_setup(connection));
        it.rem != 0;
        xcb_screen_next(&it))
    {
        auto screen = it.data;
        if(screen->root == geometryReply->root)
        {
            ret = screen;
            break;
        }
    }
    free(geometryReply);
    return ret;
}

void setNetWmState(xcb_window_t window, xcb_connection_t* connection,
    xcb_screen_t* screen,
    bool set, xcb_atom_t one, xcb_atom_t two = 0)
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = window;
    event.type = stateAtom;
    event.data.data32[0] = set? 1: 0;
    event.data.data32[1] = one;
    event.data.data32[2] = two;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;
    xcb_send_event(connection, 0, screen->root,
        XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        reinterpret_cast<const char*>(&event)
    );
}
#endif

#if _WIN32
bool isWindows11 = false;

std::once_flag getWindowsVersionFlag;

typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);

void getWindowsVersion()
{
    auto version = YADAW::Native::getWindowsVersion();
    if(version.buildVersion >= 21996) // The first leaked Windows 11 build
    {
        isWindows11 = true;
    }
}
#endif

#if _WIN32
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
#elif __APPLE__

bool isWindowResizable(QWindow& window)
{
    return true;
}


void setWindowResizable(QWindow& window, bool resizable)
{}
#elif __linux__
bool isWindowResizable(QWindow& window)
{
    return false;
}

void setWindowResizable(QWindow& window, bool resizable)
{
    auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if(x11Interface)
    {
        auto windowHandle = static_cast<xcb_window_t>(window.winId());
        auto connection = x11Interface->connection();
        if(resizable)
        {
            xcb_delete_property(connection, windowHandle, XCB_ATOM_WM_NORMAL_HINTS);
        }
        else
        {
            xcb_size_hints_t hints;
            auto getGeometryCookie = xcb_get_geometry(connection, windowHandle);
            auto geometry = xcb_get_geometry_reply(connection, getGeometryCookie, nullptr);
            xcb_icccm_size_hints_set_min_size(&hints, geometry->width, geometry->height);
            xcb_icccm_size_hints_set_max_size(&hints, geometry->width, geometry->height);
            free(geometry);
            xcb_icccm_set_wm_size_hints(connection, windowHandle, XCB_ATOM_WM_NORMAL_HINTS, &hints);
        }
    }
}
#endif

void enterFullscreen(QWindow& window)
{
    if(window.visibility() == QWindow::Visibility::Maximized)
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
#elif __APPLE__
        toggleNSWindowFullscreen(window.winId());
        fallback = false;
#elif __linux__
        auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
        if(x11Interface)
        {
            auto windowHandle = static_cast<xcb_window_t>(window.winId());
            auto connection = x11Interface->connection();
            std::call_once(initializeAtomFlag, &initializeAtoms, connection);
            auto windowScreen = getScreenOfWindow(windowHandle, connection);
            if(windowScreen)
            {
                setNetWmState(windowHandle, connection, windowScreen, true,
                    fullscreenAtom);
                fallback = false;
            }
        }
#endif
        if(fallback)
        {
            window.showFullScreen();
        }
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
        auto fallback = true;
#if _WIN32
        try
        {
            std::call_once(getWindowsVersionFlag, &getWindowsVersion);
        }
        catch(std::runtime_error& runtimeError)
        {
            qDebug(runtimeError.what());
        }
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        // We only pass the new sizing and positioning flags to the window, without
        // actually setting the window geometry.
        if(isWindows11)
        {
            window.showMinimized();
        }
        window.showMaximized();
        fallback = false;
#elif __APPLE__
        toggleNSWindowFullscreen(window.winId());
        fallback = false;
#elif __linux__
        auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
        if(x11Interface)
        {
            auto windowHandle = static_cast<xcb_window_t>(window.winId());
            auto connection = x11Interface->connection();
            std::call_once(initializeAtomFlag, &initializeAtoms, connection);
            auto windowScreen = getScreenOfWindow(windowHandle, connection);
            if(windowScreen)
            {
                setNetWmState(windowHandle, connection, windowScreen, false,
                    fullscreenAtom);
                setNetWmState(windowHandle, connection, windowScreen, true,
                    fillWidthAtom, fillHeightAtom);
                fallback = false;
            }
        }
#endif
        if(fallback)
        {
            window.showMaximized();
        }
    }
    else
    {
        window.showNormal();
    }
}
}
