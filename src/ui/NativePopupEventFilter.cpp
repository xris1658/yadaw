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

#include <cinttypes>

namespace YADAW::UI
{
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
        // We don't need to handle mouse move eventsmanually.
        if(type == QEvent::Type::MouseButtonPress
            || type == QEvent::Type::MouseButtonRelease
            || type == QEvent::Type::MouseButtonDblClick)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                auto shouldSendMousePressed = mouseEvent->globalX() < nativePopup->x()
                    || mouseEvent->globalX() > nativePopup->x() + nativePopup->width()
                    || mouseEvent->globalY() < nativePopup->y()
                    || mouseEvent->globalY() > nativePopup->y() + nativePopup->height();
                if(shouldSendMousePressed)
                {
                    auto metaObject = nativePopup->metaObject();
                    auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                    if(signalIndex != -1)
                    {
                        metaObject->method(signalIndex).invoke(nativePopup);
                    }
                }
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
        // We also don't need to handle key events manually.
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
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                auto metaObject = nativePopup->metaObject();
                auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                if(signalIndex != -1)
                {
                    metaObject->method(signalIndex).invoke(nativePopup);
                }
            }
            return false;
        }
    }
#endif
    return false;
}
}
