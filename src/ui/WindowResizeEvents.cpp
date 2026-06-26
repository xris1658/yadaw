#include "WindowResizeEvents.hpp"

#include <QCoreApplication>

// Macros that control the debug messages, uncomment these on demand
// #define YADAW_DEBUG_WINDOW_RESIZE_EVENTS_MESSAGES 1
// #define YADAW_DEBUG_WINDOW_RESIZE_EVENTS_STATES   1

namespace YADAW::UI
{
QWindow* WindowResizeEvents::window() const
{
    return windowAndId_.window;
}

bool WindowResizeEvents::resizing() const
{
    return resizing_;
}


void WindowResizeEvents::adjustRect(QRect& rect, WindowResizeEvents::DragPosition position, QSize newSize)
{
    using YADAW::UI::WindowResizeEvents;
    // Why +1/-1: https://doc.qt.io/qt-6/qrect.html#coordinates
    switch(position)
    {
    case WindowResizeEvents::DragPosition::TopLeft:
    case WindowResizeEvents::DragPosition::Invalid:
        rect.setTopLeft(QPoint(rect.right() - newSize.width(), rect.bottom() - newSize.height()));
        return;
    case WindowResizeEvents::DragPosition::Top:
    case WindowResizeEvents::DragPosition::TopRight:
        rect.setTopRight(QPoint(rect.left() + newSize.width() - 1, rect.bottom() - newSize.height()));
        return;
    case WindowResizeEvents::DragPosition::Left:
    case WindowResizeEvents::DragPosition::BottomLeft:
        rect.setBottomLeft(QPoint(rect.right() - newSize.width(), rect.top() + newSize.height() - 1));
        return;
    case WindowResizeEvents::DragPosition::Right:
    case WindowResizeEvents::DragPosition::Bottom:
    case WindowResizeEvents::DragPosition::BottomRight:
        rect.setBottomRight(QPoint(rect.left() + newSize.width() - 1, rect.top() + newSize.height() - 1));
        return;
    }
}
}