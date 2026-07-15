#if __linux__

#include "native/Shell.hpp"
#include "ui/WindowResizeEvents.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMetaMethod>

#if __linux__
#include <xcb/xproto.h>

#include <cinttypes>
#endif

// Macros that control the debug messages, uncomment these on demand
// #define YADAW_DEBUG_WINDOW_RESIZE_EVENTS_MESSAGES 1
// #define YADAW_DEBUG_WINDOW_RESIZE_EVENTS_STATES   1

namespace YADAW::UI
{
    WindowResizeEvents::DesktopNativeEventFilter WindowResizeEvents::desktopNativeEventFilter;
WindowResizeEvents::WindowResizeEvents(QWindow& window):
    windowAndId_(window)
{
    // Since different DEs have their own event patterns on resizing a window,
    // we have to check the pattern and interpret those events by myself
    // (e.g. by running `xev` and monitoring events on resizing the window).
    // I haven't taken Wayland into account for now. Sorry about that.
    if(auto desktop = YADAW::Native::getDesktop())
    {
        if(std::strstr(desktop, "KDE"))
        {
            auto sessionVersion = std::getenv("KDE_SESSION_VERSION");
            if(std::strstr(sessionVersion, "5"))
            {
                desktopNativeEventFilter = &WindowResizeEvents::nativeEventFilterOnKDE5;
            }
            else if(std::strstr(sessionVersion, "6"))
            {
                desktopNativeEventFilter = &WindowResizeEvents::nativeEventFilterOnKDE6;
            }
        }
        else if(std::strstr(desktop, "GNOME"))
        {
            desktopNativeEventFilter = &WindowResizeEvents::nativeEventFilterOnGNOME;
        }
    }
    else
    {
        desktopNativeEventFilter = &WindowResizeEvents::nativeEventFilterOnUnknown;
    }
    QCoreApplication::instance()->installNativeEventFilter(this);
}

WindowResizeEvents::~WindowResizeEvents()
{
    if(resizing_)
    {
        resized(windowAndId_.window->geometry());
        endResize();
    }
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

WindowResizeEvents::FeatureSupportFlags WindowResizeEvents::getNativeSupportFlags()
{
    // On KDE 5:
    // - No events are sent on starting/ending resizing.
    // - Two `XCB_CONFIGURE_NOTIFY` events are sent while resizing a
    //   window. Both events store the new window size.
    //   `QWindow::geometry()` returns the old size on receiving the first one.
    //   (This might change if Qt updates their implementations.)
    //
    // `WindowResizeEvents`:
    // - Emits `aboutToResize()` on receiving the first `XCB_CONFIGURE_NOTIFY`
    //   event;
    // - Emits `resized()` on receiving the second `XCB_CONFIGURE_NOTIFY`.
    if(desktopNativeEventFilter == &WindowResizeEvents::nativeEventFilterOnKDE5)
    {
        return FeatureSupportFlag::SupportsAboutToResize
            |  FeatureSupportFlag::SupportsResized
            |  FeatureSupportFlag::UsesPhysicalSize;
    }
    // On KDE 5:
    // - No events are sent on starting/ending resizing.
    // - One `XCB_CONFIGURE_NOTIFY` event is sent while resizing a window.
    //   This event store the new window size.
    //
    // `WindowResizeEvents`:
    // - Emits `resized()` on receiving the second `XCB_CONFIGURE_NOTIFY`.
    if(desktopNativeEventFilter == &WindowResizeEvents::nativeEventFilterOnKDE6)
    {
        return FeatureSupportFlag::SupportsResized
            |  FeatureSupportFlag::UsesPhysicalSize;
    }
    // On GNOME:
    // - An `XCB_FOCUS_OUT` event is sent on starting resizing, and an
    //   `XCB_FOCUS_IN` event on ending resizing. The first event is sent
    //   whether the window has focus or not on starting resizing.
    // - Only one `XCB_CONFIGURE_NOTIFY` event is sent during resizing, and
    //   `QWindow::geometry()` returns the old size on receiving this event.
    //   (This might change if Qt updates their implementations.)
    //
    // `WindowResizeEvents`:
    // - Emits `startResizing()` on receiving the first `XCB_CONFIGURE_NOTIFY`
    //   event after the `XCB_FOCUS_OUT` event;
    // - Emits `aboutToResize()` on receiving the event. Of course,
    //   `aboutToResize()` is emitted AFTER `startResizing()`.
    // - Emits `stopResizing()` on receiving the `XCB_FOCUS_IN` event;
    if(desktopNativeEventFilter == &WindowResizeEvents::nativeEventFilterOnGNOME)
    {
        return FeatureSupportFlag::SupportsStartAndEndResize
            |  FeatureSupportFlag::SupportsAboutToResize
            |  FeatureSupportFlag::SupportsResized;
    }
    // TODO: Add status and events sent of other DEs
    return 0;
}

bool WindowResizeEvents::nativeEventFilterOnKDE5(xcb_generic_event_t* event)
{
    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
        &WindowResizeEvents::aboutToResize
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
                aboutToResize(WindowResizeEvents::DragPosition::BottomRight, &newGeometry);
            }
            else
            {
                resized(geometry);
            }
        }
    }
    return false;
}

bool WindowResizeEvents::nativeEventFilterOnKDE6(xcb_generic_event_t* event)
{
    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
        &WindowResizeEvents::aboutToResize
    );
    auto responseType = event->response_type & 0x7F;
    if(responseType == XCB_CONFIGURE_NOTIFY)
    {
        auto configureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t*>(event);
#if YADAW_DEBUG_WINDOW_RESIZE_EVENTS_MESSAGES
        std::fprintf(
            stderr, "[DEBUG] WindowResizeEvents: event: %" PRIx32"; window: %" PRIx32"; above_sibling:  %" PRIx32"\n",
            configureNotifyEvent->event, configureNotifyEvent->window, configureNotifyEvent->above_sibling
        );
        std::fprintf(
            stderr, "[DEBUG] WindowResizeEvents: (%" PRId16", %" PRId16"), %" PRIu16" x %" PRIu16"\n",
            configureNotifyEvent->x, configureNotifyEvent->y, configureNotifyEvent->width, configureNotifyEvent->height
        );
#endif
        if(configureNotifyEvent->event == windowAndId_.winId || configureNotifyEvent->window == windowAndId_.winId)
        {
            QRect newGeometry(
                configureNotifyEvent->x, configureNotifyEvent->y,
                configureNotifyEvent->width, configureNotifyEvent->height
            );
            resized(newGeometry);
        }
    }
    return false;
}

bool WindowResizeEvents::nativeEventFilterOnGNOME(xcb_generic_event_t* event)
{
    static auto aboutToResizeSignal = QMetaMethod::fromSignal(
        &WindowResizeEvents::aboutToResize
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
            aboutToResize(WindowResizeEvents::DragPosition::BottomRight, &newGeometry);
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

bool WindowResizeEvents::nativeEventFilterOnUnknown(xcb_generic_event_t* event)
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
            aboutToResize(WindowResizeEvents::DragPosition::BottomRight, &newGeometry);
        }
    }
    else if(responseType == XCB_EXPOSE && lastResponseType_ == XCB_CONFIGURE_NOTIFY)
    {
        resized(windowAndId_.window->geometry());
    }
    return false;
}

bool WindowResizeEvents::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
    if(eventType == "xcb_generic_event_t")
    {
        auto event = static_cast<xcb_generic_event_t*>(message);
        auto func = desktopNativeEventFilter;
        (this->*func)(event);
    }
    return false;
}
}

#endif