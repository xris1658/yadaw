#include "ExposeAndPaintEventFilter.hpp"

#include <QEvent>
#include <QExposeEvent>
#include <QObject>
#include <QPaintEvent>
#include <QVariant>
#include <QWindow>

#include <cinttypes>

namespace YADAW::UI
{
ExposeAndPaintEventFilter::ExposeAndPaintEventFilter(QWindow& window):
    QObject(&window),
    window_(&window)
{
    window.setProperty("exposeEventFilter", QVariant::fromValue<QObject*>(this));
    window.installEventFilter(this);
}

ExposeAndPaintEventFilter::~ExposeAndPaintEventFilter()
{
    window_->removeEventFilter(this);
}

ExposeAndPaintEventFilter& ExposeAndPaintEventFilter::createIfNeeded(QWindow& window)
{
    auto ret = window.property("exposeEventFilter").value<QObject*>();
    if(!ret)
    {
        return *(new ExposeAndPaintEventFilter(window));
    }
    return *static_cast<ExposeAndPaintEventFilter*>(ret);
}

bool ExposeAndPaintEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == window_)
    {
        if(auto type = event->type(); type == QEvent::Expose)
        {
            auto exposeEvent = static_cast<QExposeEvent*>(event);
            if(exposeEvent->region() != prevRegion_)
            {
                prevRegion_ = exposeEvent->region();
                return false;
            }
            exposeEvent->ignore();
            return true;
        }
        else if(type == QEvent::Paint)
        {
            auto paintEvent = static_cast<QPaintEvent*>(event);
            if(paintEvent->region() != prevRegion_)
            {
                prevRegion_ = paintEvent->region();
                return false;
            }
            paintEvent->ignore();
            return true;
        }
    }
    return false;
}
}
