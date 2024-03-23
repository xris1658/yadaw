#include "UI.hpp"

#include "native/Native.hpp"

#include <QCoreApplication>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

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

void setWindowFullScreen(QWindow& window, bool fullscreen, bool maximized)
{
    bool fallback = true;
#if __linux__
    const char* fullscreenText = "_NET_WM_STATE_FULLSCREEN\0";
    const char* maximizeWidthText = "_NET_WM_STATE_MAXIMIZED_VERT\0";
    const char* maximizeHeightText = "_NET_WM_STATE_MAXIMIZED_HORZ\0";
    auto winId = window.winId();
    auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if(x11Interface)
    {
        auto connection = x11Interface->connection();
        auto windowHandle = static_cast<xcb_window_t>(window.winId());
        xcb_client_message_event_t event;
        event.response_type = XCB_CLIENT_MESSAGE;
        event.format = 32;
        event.sequence = 0;
        event.window = windowHandle;
        event.data.data32[0] = 1;
        event.data.data32[3] = 0;
        event.data.data32[4] = 0;
        if(fullscreen)
        {
            auto fullscreenCookie = xcb_intern_atom(connection, false,
                strlen(fullscreenText), fullscreenText);
            auto fullscreenAtomReply = xcb_intern_atom_reply(
                connection, fullscreenCookie, nullptr);
            event.data.data32[1] = fullscreenAtomReply->atom;
            event.data.data32[2] = 0;
            free(fullscreenAtomReply);
        }
        else
        {
            if(maximized)
            {
                auto maximizeWidthCookie = xcb_intern_atom(
                    connection, false,
                    strlen(maximizeWidthText), maximizeWidthText);
                auto maximizeWidthAtomReply = xcb_intern_atom_reply(
                    connection, maximizeWidthCookie, nullptr
                );
                event.data.data32[1] = maximizeWidthAtomReply->atom;
                auto maximizeHeightCookie = xcb_intern_atom(
                    connection, false,
                    strlen(maximizeHeightText), maximizeHeightText
                );
                auto maximizeHeightAtomReply = xcb_intern_atom_reply(
                    connection, maximizeHeightCookie, nullptr
                );
                event.data.data32[2] = maximizeHeightAtomReply->atom;
                free(maximizeWidthAtomReply);
                free(maximizeHeightAtomReply);
            }
        }
        xcb_send_event(
            connection, 0, windowHandle,
            XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
            reinterpret_cast<const char*>(&event)
        );
        // fallback = false
    }
#endif
    if(fallback)
    {
        if(fullscreen)
        {
            window.showFullScreen();
        }
        else
        {
            if(maximized)
            {
                window.showMaximized();
            }
            else
            {
                window.showNormal();
            }
        }
    }
}
}
