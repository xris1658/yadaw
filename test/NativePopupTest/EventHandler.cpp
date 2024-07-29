#include "EventHandler.hpp"

#include "ui/UI.hpp"

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
    YADAW::UI::showWindowWithoutActivating(nativePopup->winId());
}
