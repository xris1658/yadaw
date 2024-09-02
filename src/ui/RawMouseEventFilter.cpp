#include "RawMouseEventFilter.hpp"

#include <QMouseEvent>

#if _WIN32
#include <Windows.h>
#endif

namespace YADAW::UI
{
RawMouseEventFilter::RawMouseEventFilter(QWindow& window):
    windowAndId_(window)
{}

RawMouseEventFilter::~RawMouseEventFilter()
{
    endHandlingRawMouseEvent();
    endLockCursor();
    endHideCursor();
}

void RawMouseEventFilter::beginHideCursor()
{
#if _WIN32
    ShowCursor(FALSE);
#endif
}

void RawMouseEventFilter::endHideCursor()
{
#if _WIN32
    ShowCursor(TRUE);
#endif
}

void RawMouseEventFilter::beginLockCursor(const QPoint& point)
{
 #if _WIN32
    RECT rect;
    rect.left = point.x();
    rect.top = point.y();
    rect.right = rect.left + 1;
    rect.bottom = rect.top + 1;
    ClipCursor(&rect);
#endif
}

void RawMouseEventFilter::endLockCursor()
{
#if _WIN32
    ClipCursor(NULL);
#endif
}

void RawMouseEventFilter::beginHandlingRawMouseEvent()
{
    if(!handlingRawMouseEvent_)
    {
#if _WIN32
        POINT point;
        GetCursorPos(&point);
        prevX_ = point.x;
        prevY_ = point.y;
        RAWINPUTDEVICE rawInputDevice {
            0x0001,
            0x0002,
            0,
            reinterpret_cast<HWND>(windowAndId_.winId)
        };
        RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
        handlingRawMouseEvent_ = true;
#endif
    }
}

void RawMouseEventFilter::endHandlingRawMouseEvent()
{
    if(handlingRawMouseEvent_)
    {
#if _WIN32
        RAWINPUTDEVICE rawInputDevice {
            0x0001,
            0x0002,
            RIDEV_REMOVE,
            reinterpret_cast<HWND>(windowAndId_.winId)
        };
        RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
        handlingRawMouseEvent_ = false;
#endif
    }
}

bool RawMouseEventFilter::isHandlingRawMouseEvent() const
{
    return handlingRawMouseEvent_;
}

QPoint RawMouseEventFilter::rawCursorPosition() const
{
    return QPoint(prevX_, prevY_);
}

bool RawMouseEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if(isHandlingRawMouseEvent())
        {
            endHandlingRawMouseEvent();
#if _WIN32
            auto point = rawCursorPosition();
            POINT nativePoint {point.x(), point.y()};
            MapWindowPoints(
                HWND_DESKTOP,
                reinterpret_cast<HWND>(windowAndId_.winId),
                &nativePoint, 1
            );
            QPointF localPoint(nativePoint.x, nativePoint.y);
            QPointF globalPoint(point.x(), point.y());
            QMouseEvent newPointMouseEvent(
                QEvent::MouseButtonRelease,
                localPoint, globalPoint,
                mouseEvent->button(),
                mouseEvent->buttons(),
                mouseEvent->modifiers(),
                mouseEvent->pointingDevice()
            );
            return watched->event(&newPointMouseEvent);
#endif
        }
    }
    return QObject::eventFilter(watched, event);
}

bool RawMouseEventFilter::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
    if(handlingRawMouseEvent_)
    {
#if _WIN32
        if(eventType == "windows_generic_MSG")
        {
            auto msg = static_cast<MSG*>(message);
            if(msg->message == WM_INPUT
                && msg->hwnd == reinterpret_cast<HWND>(windowAndId_.winId)
            )
            {
                auto rawInputHandle = reinterpret_cast<HRAWINPUT>(msg->lParam);
                RAWINPUT rawInput;
                UINT rawInputSize = sizeof(rawInput);
                auto bytesCopied = GetRawInputData(
                    rawInputHandle, RID_INPUT, &rawInput, &rawInputSize, sizeof(RAWINPUTHEADER)
                );
                if(bytesCopied != UINT(-1) && rawInput.header.dwType == RIM_TYPEMOUSE)
                {
                    const auto& rawMouse = rawInput.data.mouse;
                    bool isMouseMove = false;
                    POINT nativeLocalPos;
                    POINT nativeGlobalPos;
                    if(rawMouse.usFlags == MOUSE_MOVE_RELATIVE)
                    {
                        nativeGlobalPos.x = prevX_ + rawMouse.lLastX;
                        nativeGlobalPos.y = prevY_ + rawMouse.lLastY;
                        isMouseMove = true;
                    }
                    else if(rawMouse.usFlags == MOUSE_MOVE_ABSOLUTE)
                    {
                        nativeGlobalPos.x = rawMouse.lLastX;
                        nativeGlobalPos.y = rawMouse.lLastY;
                        isMouseMove = true;
                    }
                    if(isMouseMove)
                    {
                        prevX_ = nativeGlobalPos.x;
                        prevY_ = nativeGlobalPos.y;
                        // qDebug("%u, %u", prevX, prevY);
                        nativeLocalPos.x = nativeGlobalPos.x;
                        nativeLocalPos.y = nativeGlobalPos.y;
                        MapWindowPoints(
                            HWND_DESKTOP,
                            reinterpret_cast<HWND>(windowAndId_.winId),
                            &nativeLocalPos, 1
                        );
                        QPointF localPos(nativeLocalPos.x, nativeLocalPos.y);
                        QPointF globalPos(nativeGlobalPos.x, nativeGlobalPos.y);
                        QMouseEvent mouseEvent(
                            QEvent::Type::MouseMove,
                            localPos,
                            globalPos,
                            Qt::MouseButton::NoButton,
                            Qt::MouseButton::NoButton,
                            Qt::KeyboardModifier::NoModifier
                        );
                        static_cast<QObject*>(windowAndId_.window)->event(&mouseEvent);
                        if(result)
                        {
                            *result = 0;
                        }
                        return true;
                    }
                }
            }
        }
#endif
    }
    return false;
}
}
