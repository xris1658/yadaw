#include "EventHandler.hpp"

#include <QVariant>

#if _WIN32
#include <windows.h>
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
#endif
    nativePopup->setVisible(true);
}
