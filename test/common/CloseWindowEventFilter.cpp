#include "CloseWindowEventFilter.hpp"

#include <QCloseEvent>

CloseWindowEventFilter::CloseWindowEventFilter()
{
}

CloseWindowEventFilter::~CloseWindowEventFilter()
{
}

void CloseWindowEventFilter::setWindow(QWindow& window)
{
    if(window_)
    {
        window_->removeEventFilter(this);
    }
    window_ = &window;
    window_->installEventFilter(this);
}

void CloseWindowEventFilter::setClose(bool close)
{
    close_ = close;
}

bool CloseWindowEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::Close)
    {
        aboutToClose();
    }
    if(close_)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
    return QObject::eventFilter(watched, event);
}
