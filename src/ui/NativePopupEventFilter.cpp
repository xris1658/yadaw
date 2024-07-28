#include "NativePopupEventFilter.hpp"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QQuickWindow>

#include "util/IntegerRange.hpp"

#if _WIN32
#include <windows.h>
#include <windowsx.h>
#elif __linux__
#include <xcb/xcb.h>
#endif

namespace YADAW::UI
{
void showWindowWithoutActivating(QWindow& window)
{
#if _WIN32
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
#endif
}

NativePopupEventFilter::NativePopupEventFilter(QWindow& parentWindow):
    QObject(nullptr),
    QAbstractNativeEventFilter(),
    parentWindow_(parentWindow)
{
    parentWindow.installEventFilter(this);
    QCoreApplication::instance()->installNativeEventFilter(this);
}

NativePopupEventFilter::~NativePopupEventFilter()
{
    QCoreApplication::instance()->removeNativeEventFilter(this);
    parentWindow_.window->removeEventFilter(this);
}

const QWindow& NativePopupEventFilter::parentWindow() const
{
    return *parentWindow_.window;
}

QWindow& NativePopupEventFilter::parentWindow()
{
    return *parentWindow_.window;
}

bool NativePopupEventFilter::empty() const
{
    return nativePopups_.empty();
}

std::uint32_t NativePopupEventFilter::count() const
{
    return nativePopups_.size();
}

const QWindow& NativePopupEventFilter::operator[](std::uint32_t index) const
{
    return *nativePopups_[index].window;
}

QWindow& NativePopupEventFilter::operator[](std::uint32_t index)
{
    return *nativePopups_[index].window;
}

const QWindow& NativePopupEventFilter::at(std::uint32_t index) const
{
    return *nativePopups_.at(index).window;
}

QWindow& NativePopupEventFilter::at(std::uint32_t index)
{
    return *nativePopups_.at(index).window;
}

bool NativePopupEventFilter::insert(QWindow& nativePopup, std::uint32_t index)
{
    if(index < count())
    {
        nativePopups_.emplace(nativePopups_.begin() + index, nativePopup);
        QObject::connect(
            this, SIGNAL(mousePressedOutside()),
            &nativePopup, SIGNAL(mousePressedOutside())
        );
        return true;
    }
    return false;
}

void NativePopupEventFilter::append(QWindow& nativePopup)
{
    nativePopups_.emplace_back(nativePopup);
    QObject::connect(
        this, SIGNAL(mousePressedOutside()),
        &nativePopup, SIGNAL(mousePressedOutside()),
        Qt::ConnectionType::QueuedConnection
    );
}

bool NativePopupEventFilter::remove(std::uint32_t index, std::uint32_t removeCount)
{
    auto count = this->count();
    if(index < count && index + removeCount <= count)
    {
        FOR_RANGE(i, index, index + removeCount)
        {
            QObject::disconnect(
                this, &NativePopupEventFilter::mousePressedOutside,
                nativePopups_[i].window, nullptr
            );
            nativePopups_[i].window->close();
        }
        nativePopups_.erase(
            nativePopups_.begin() + index,
            nativePopups_.begin() + index + removeCount
        );
        return true;
    }
    return false;
}

void NativePopupEventFilter::clear()
{
    for(auto& [window, winId]: nativePopups_)
    {
        window->close();
    }
    nativePopups_.clear();
}

bool NativePopupEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == parentWindow_.window)
    {
        auto type = event->type();
        if(type == QEvent::Type::MouseMove)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                QMouseEvent mouseEventForPopup(
                    type,
                    QPointF(
                        mouseEvent->globalPosition().x() - nativePopup->x(),
                        mouseEvent->globalPosition().y() - nativePopup->y()
                    ),
                    mouseEvent->button(),
                    mouseEvent->buttons(),
                    mouseEvent->modifiers(),
                    mouseEvent->pointingDevice()
                );
                ret |= static_cast<QObject*>(nativePopup)->event(&mouseEventForPopup);
            }
            return ret || watched->event(event);
        }
        else if(type == QEvent::Type::MouseButtonPress
            || type == QEvent::Type::MouseButtonRelease
            || type == QEvent::Type::MouseButtonDblClick)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            auto shouldSendMousePressed = std::all_of(
                nativePopups_.begin(), nativePopups_.end(),
                [&mouseEvent](WindowAndId& windowAndId)
                {
                    auto& [window, winId] = windowAndId;
                    return mouseEvent->globalX() < window->x()
                    || mouseEvent->globalX() > window->x() + window->width()
                    || mouseEvent->globalY() < window->y()
                    || mouseEvent->globalY() > window->y() + window->height();
                }
            );
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                QMouseEvent mouseEventForPopup(
                    type,
                    QPointF(
                        mouseEvent->globalPosition().x() - nativePopup->x(),
                        mouseEvent->globalPosition().y() - nativePopup->y()
                    ),
                    mouseEvent->button(),
                    mouseEvent->buttons(),
                    mouseEvent->modifiers(),
                    mouseEvent->pointingDevice()
                );
                ret |= static_cast<QObject*>(nativePopup)->event(&mouseEventForPopup);
            }
            if(shouldSendMousePressed)
            {
                mousePressedOutside();
                return watched->event(event);
            }
            return ret || watched->event(event);
        }
        else if(type == QEvent::Type::Gesture
            || type == QEvent::Type::GestureOverride
            || type == QEvent::Type::NativeGesture
            || type == QEvent::Type::TouchBegin
            || type == QEvent::Type::TouchCancel
            || type == QEvent::Type::TouchEnd
            || type == QEvent::Type::TouchUpdate)
        {
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                ret |= static_cast<QObject*>(nativePopup)->event(event);
            }
            return ret | watched->event(event);
        }
        else if(type == QEvent::Type::KeyPress
            || type == QEvent::Type::KeyRelease)
        {
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                // An inactive `QQuickWindow` will not process key events. So
                // it's useless to forward those events.
                // (See https://doc.qt.io/qt-6/qtquick-input-focus.html#key-handling-overview)
                if(auto quickWindow = qobject_cast<QQuickWindow*>(nativePopup);
                    (!quickWindow) || quickWindow->isActive())
                {
                    ret |= static_cast<QObject*>(nativePopup)->event(event);
                }
            }
            return ret | watched->event(event);
        }
    }
    return false;
}

bool NativePopupEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
#if _WIN32
    if(eventType == "windows_generic_MSG")
    {
    auto msg = static_cast<MSG*>(message);
        // Mouse click in the native popups
        if(msg->message == WM_MOUSEACTIVATE)
        {
            for(auto& [window, winId]: nativePopups_)
            {
                if(msg->hwnd == reinterpret_cast<HWND>(winId))
                {
                    *result = MA_NOACTIVATE;
                    return true;
                }
            }
        }
        // Mouse click in the parent window's non-client area, e.g. title bar
        // `WM_NCLBUTTONDOWN` starts dragging and moving the window;
        // `WM_NCRBUTTONUP` after `WM_NCRBUTTONDOWN` shows the control menu
        else if(
            (msg->message == WM_NCLBUTTONDOWN || msg->message == WM_NCRBUTTONDOWN)
            && msg->hwnd == reinterpret_cast<HWND>(parentWindow_.winId)
        )
        {
            auto x = GET_X_LPARAM(msg->lParam);
            auto y = GET_Y_LPARAM(msg->lParam);
            auto shouldSendMousePressed = std::all_of(
                nativePopups_.begin(), nativePopups_.end(),
                [x, y](WindowAndId& windowAndId)
                {
                    auto& [window, winId] = windowAndId;
                    return x < window->x()
                    || x > window->x() + window->width()
                    || y < window->y()
                    || y > window->y() + window->height();
                }
            );
            if(shouldSendMousePressed)
            {
                mousePressedOutside();
            }
            return false;
        }
    }
#elif __linux__
    if(eventType == "xcb_generic_event_t")
    {
        auto event = static_cast<xcb_generic_event_t*>(message);
        // Use the event
        // if(event->response_type == XCB_EXPOSE)
        // {
        //     auto exposeEvent = reinterpret_cast<xcb_expose_event_t*>(event);
        //     for(auto& [nativePopup, winId]: nativePopups_)
        //     {
        //         if(exposeEvent->window == static_cast<xcb_window_t>(winId))
        //         {
        //             return true;
        //         }
        //     }
        // }
        qDebug() << event->full_sequence << event->response_type;
    }
#endif
    return false;
}
}
