#if __linux__

#include "native/Window.hpp"

#include <QGuiApplication>

#include <X11/Xutil.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>

#include <mutex>



namespace YADAW::Native
{

void showWindowWithoutActivating(QWindow& window)
{
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
    window.setVisible(true);
}

QRect getPhysicalGeometry(QWindow& window)
{
    if(auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
    {
        auto windowHandle = static_cast<xcb_window_t>(window.winId());
        auto connection = x11Interface->connection();
        auto cookie = xcb_get_geometry(connection, windowHandle);
        xcb_generic_error_t* pError = nullptr;
        if(auto reply = xcb_get_geometry_reply(connection, cookie, &pError))
        {
            auto ret = QRect(reply->x, reply->y, reply->width, reply->height);
            free(reply);
            return ret;
        }
        else if(pError)
        {
            free(pError);
        }
    }
    return {};
}

xcb_atom_t stateAtom;
xcb_atom_t fullscreenAtom;
xcb_atom_t fillWidthAtom;
xcb_atom_t fillHeightAtom;
xcb_atom_t frameMarginsAtom;
const char stateText[] = "_NET_WM_STATE";
const char fullscreenText[] = "_NET_WM_STATE_FULLSCREEN";
const char fillWidthText[] = "_NET_WM_STATE_MAXIMIZED_HORZ";
const char fillHeightText[] = "_NET_WM_STATE_MAXIMIZED_VERT";
const char resizeText[] = "_NET_WM_ACTION_RESIZE";
const char changeStateText[] = "WM_CHANGE_STATE";
const char frameMarginsText[] = "_NET_FRAME_EXTENTS";

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
    auto frameMarginCookie = xcb_intern_atom(connection, false,
        strlen(frameMarginsText), frameMarginsText);
    reply = xcb_intern_atom_reply(connection, frameMarginCookie, nullptr);
    frameMarginsAtom = reply->atom;
    free(reply);
}

QMargins getPhysicalFrameMargin(QWindow& window)
{
    if(auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
    {
        auto windowHandle = static_cast<xcb_window_t>(window.winId());
        auto connection = x11Interface->connection();
        // Mostly copied from `QXcbWindow::frameMargins()` (qtbase/src/plugins/platforms/xcb/qxcbwindow.cpp)
        auto getPropertyCookie = xcb_get_property(
            connection, false, windowHandle, frameMarginsAtom, XCB_ATOM_CARDINAL, 0, 4
        );
        xcb_generic_error_t* pError = nullptr;
        if(auto propertyReply = xcb_get_property_reply(connection, getPropertyCookie, &pError);
            propertyReply && propertyReply->type == XCB_ATOM_CARDINAL && propertyReply->format == 32 && propertyReply->value_len == 4)
        {
            auto* data = reinterpret_cast<std::uint32_t*>(xcb_get_property_value(propertyReply));
            QMargins ret(data[0], data[2], data[1], data[3]);
            free(propertyReply);
            return ret;
        }
        else if(pError)
        {
            free(pError);
        }
    }
    return {};
}

QRect getPhysicalFrameGeometry(QWindow& window)
{
    return getPhysicalGeometry(window).marginsAdded(getPhysicalFrameMargin(window));
}

enum SetPhysicalGeometryFlag: std::uint8_t
{
    PhysicalSize     = 1 << 0,
    PhysicalPosition = 1 << 1
};

using SetPhysicalGeometryFlags = std::underlying_type_t<SetPhysicalGeometryFlag>;

constexpr std::int32_t coordMax = std::numeric_limits<std::uint16_t>::max();

bool setPhysicalGeometry(QWindow& window, const QRect& physicalGeometry, SetPhysicalGeometryFlags mask)
{
    if(auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
    {
        auto windowHandle = static_cast<xcb_window_t>(window.winId());
        auto connection = x11Interface->connection();
        std::int32_t values[4] = {
            static_cast<std::int32_t>(std::clamp(physicalGeometry.x(),      -coordMax, coordMax)),
            static_cast<std::int32_t>(std::clamp(physicalGeometry.y(),      -coordMax, coordMax)),
            static_cast<std::int32_t>(std::clamp(physicalGeometry.width(),  1,         coordMax)),
            static_cast<std::int32_t>(std::clamp(physicalGeometry.height(), 1,         coordMax))
        };
        xcb_generic_error_t* pError = nullptr;
        xcb_void_cookie_t configureWindowCookie;
        if(mask == (SetPhysicalGeometryFlag::PhysicalSize | SetPhysicalGeometryFlag::PhysicalPosition))
        {
            configureWindowCookie = xcb_configure_window_checked(
                connection, windowHandle,
                XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values
            );
        }
        else if(mask == SetPhysicalGeometryFlag::PhysicalPosition)
        {
            configureWindowCookie = xcb_configure_window_checked(
                connection, windowHandle,
                XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values
            );
        }
        else if(mask == SetPhysicalGeometryFlag::PhysicalSize)
        {
            configureWindowCookie = xcb_configure_window_checked(
                connection, windowHandle,
                XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values + 2
            );
        }
        if(mask != 0)
        {
            pError = xcb_request_check(connection, configureWindowCookie);
            if(!pError)
            {
                return true;
            }
            else
            {
                free(pError);
                return false;
            }
        }
    }
    return false;
}

bool setPhysicalGeometry(QWindow& window, const QRect& physicalGeometry)
{
    return setPhysicalGeometry(
        window, physicalGeometry, SetPhysicalGeometryFlag::PhysicalPosition | SetPhysicalGeometryFlag::PhysicalSize
    );
}

bool setPhysicalSize(QWindow& window, const QSize& physicalSize)
{
    return setPhysicalGeometry(
        window, QRect(QPoint(0, 0), physicalSize), SetPhysicalGeometryFlag::PhysicalSize
    );
}

bool setPhysicalPosition(QWindow& window, const QPoint& physicalPosition)
{
    return setPhysicalGeometry(
        window, QRect(physicalPosition, QSize(0, 0)), SetPhysicalGeometryFlag::PhysicalPosition
    );
}

bool setPhysicalFramePosition(QWindow& window, const QPoint& physicalPosition)
{
    auto frameMargin = getPhysicalFrameMargin(window);
    return setPhysicalPosition(window, physicalPosition - QPoint(frameMargin.left(), frameMargin.top()));
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

bool isWindowResizableByUser(QWindow& window)
{
    return false;
}

    void setWindowResizableByUser(QWindow& window, bool resizable)
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
            // Sometimes `xcb_get_geometry` return old results if it is called
            // immediately after a `QWindow::resize()`. This causes a lot of
            // trouble while showing a plugin window which is not resizable by
            // user. Since I don't retrieve window size from non-native code
            // by calling `xcb_get_geometry`, I'll just retrieve window size
            // from Qt instead.
            // (For those who're wondering, yes, I've tried calling `xcb_flush`
            // here, but the window size was still inconsistent.)
            auto geometry = window.size() * window.devicePixelRatio();
            xcb_icccm_size_hints_set_min_size(&hints, geometry.width(), geometry.height());
            xcb_icccm_size_hints_set_max_size(&hints, geometry.width(), geometry.height());
            xcb_icccm_set_wm_size_hints(connection, windowHandle, XCB_ATOM_WM_NORMAL_HINTS, &hints);
        }
    }
}

bool isWindowMaximized(QWindow& window)
{
    return window.visibility() == QWindow::Visibility::Maximized;
}

void enterFullscreen(QWindow& window)
{
    if(window.visibility() == QWindow::Visibility::Maximized)
    {
        auto fallback = true;
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

#endif