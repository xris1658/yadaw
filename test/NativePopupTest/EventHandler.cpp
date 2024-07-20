#include "EventHandler.hpp"

#include <QVariant>

#if _WIN32
#include <windows.h>
#elif __linux__
#include <QGuiApplication>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

EventHandler::EventHandler(QWindow& mainWindow):
    mainWindow_(&mainWindow)
{
    QObject::connect(
        mainWindow_, SIGNAL(showNativePopup(int, int)),
        this, SLOT(onShowNativePopup(int, int))
    );
}

EventHandler::~EventHandler()
{}

void EventHandler::onShowNativePopup(int x, int y)
{
    auto nativePopup = static_cast<QWindow*>(mainWindow_->property("nativePopup").value<QObject*>());
    nativePopup->setX(x);
    nativePopup->setY(y);
#if _WIN32
    auto hwnd = reinterpret_cast<HWND>(nativePopup->winId());
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
#elif __linux__
    auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if(x11Interface)
    {
        auto windowHandle = static_cast<xcb_window_t>(nativePopup->winId());
        auto connection = x11Interface->connection();
        auto cookie = xcb_map_window(connection, windowHandle);
        qDebug() << "onShowNativePopup generated this cookie: " << cookie.sequence;
    }
#endif
    nativePopup->setVisible(true);
}
