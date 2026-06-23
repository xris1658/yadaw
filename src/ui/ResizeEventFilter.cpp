#include "ResizeEventFilter.hpp"

#include <QCoreApplication>

// Macros that control the debug messages, uncomment these on demand
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES 1
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES   1

namespace YADAW::UI
{
QWindow* ResizeEventFilter::window() const
{
    return windowAndId_.window;
}

bool ResizeEventFilter::resizing() const
{
    return resizing_;
}


void ResizeEventFilter::adjustRect(QRect& rect, ResizeEventFilter::DragPosition position, QSize newSize)
{
    using YADAW::UI::ResizeEventFilter;
    // Why +1/-1: https://doc.qt.io/qt-6/qrect.html#coordinates
    switch(position)
    {
    case ResizeEventFilter::DragPosition::TopLeft:
    case ResizeEventFilter::DragPosition::Invalid:
        rect.setTopLeft(QPoint(rect.right() - newSize.width(), rect.bottom() - newSize.height()));
        return;
    case ResizeEventFilter::DragPosition::Top:
    case ResizeEventFilter::DragPosition::TopRight:
        rect.setTopRight(QPoint(rect.left() + newSize.width() - 1, rect.bottom() - newSize.height()));
        return;
    case ResizeEventFilter::DragPosition::Left:
    case ResizeEventFilter::DragPosition::BottomLeft:
        rect.setBottomLeft(QPoint(rect.right() - newSize.width(), rect.top() + newSize.height() - 1));
        return;
    case ResizeEventFilter::DragPosition::Right:
    case ResizeEventFilter::DragPosition::Bottom:
    case ResizeEventFilter::DragPosition::BottomRight:
        rect.setBottomRight(QPoint(rect.left() + newSize.width() - 1, rect.top() + newSize.height() - 1));
        return;
    }
}
}