#include "PluginWindow.hpp"

#include "native/Window.hpp"

namespace YADAW::Audio::Plugin
{
using YADAW::UI::ResizeEventFilter;

PluginWindow::PluginWindow(
        YADAW::Audio::Plugin::IPluginGUI& pluginGUI,
        QWindow* parent):
    QWindow(parent),
    pluginFrame_(this),
    resizeEventFilter_(*this),
    pluginGUI_(&pluginGUI)
{
    pluginGUI_->attachToWindow(&pluginFrame_);
    YADAW::Native::setWindowResizableByUser(
        *this, pluginGUI_->resizableByUser()
    );
}

PluginWindow::~PluginWindow()
{
    pluginGUI_->detachWithWindow();
}

QWindow& PluginWindow::pluginFrame()
{
    return pluginFrame_;
}

QWindow* PluginWindow::topBar()
{
    return topBar_;
}

void PluginWindow::setTopBar(QWindow* bar)
{
    if(bar == nullptr && topBar_ != nullptr)
    {
        resizeOps_ |= ResizeOp::Repositioning;
        pluginFrame_.setPosition(0, 0);
        setHeight(height() - topBar_->height());
        topBar_ = bar;
        resizeOps_ ^= ResizeOp::Repositioning;
    }
    else if(bar != nullptr && bar != topBar_)
    {
        resizeOps_ |= ResizeOp::Repositioning;
        pluginFrame_.setPosition(0, bar->height());
        setHeight(height() + bar->height());
        topBar_ = bar;
        resizeOps_ ^= ResizeOp::Repositioning;
    }
}

void PluginWindow::resizeFromPlugin(const QSize& size)
{
    resizeOps_ |= ResizeOp::ResizingFromPlugin;
    resize(size.width(), size.height() + pluginFrame_.y());
    resizeOps_ ^= ResizeOp::ResizingFromPlugin;
}

void PluginWindow::onStartResize()
{
    if(resizeOps_ == 0)
    {
        // TODO
    }
}

void PluginWindow::onAboutToResize(YADAW::UI::ResizeEventFilter::DragPosition dragPosition, QRect* rect)
{
    if(resizeOps_ == 0)
    {
        if((ResizeEventFilter::getNativeSupportFlags()
            & ResizeEventFilter::FeatureSupportFlag::SupportsAdjustOnAboutToResize))
        {
            assert(YADAW::Native::isWindowResizableByUser(*this));
            auto size = rect->size();
            if(auto adjustSizeResult = pluginGUI_->adjustSize(size))
            {
                YADAW::UI::ResizeEventFilter::adjustRect(*rect, dragPosition, size);
            }
        }
    }
}

void PluginWindow::onResized(QRect rect)
{
    if(resizeOps_ == 0)
    {
        if((ResizeEventFilter::getNativeSupportFlags()
            & ResizeEventFilter::FeatureSupportFlag::SupportsAdjustOnAboutToResize))
        {
            auto frameSize = rect.size();
            frameSize.setHeight(
                frameSize.height() - (topBar_? topBar_->height(): 0)
            );
            pluginFrame_.resize(frameSize);
        }
    }
}

void PluginWindow::onEndResize()
{
    if(resizeOps_ == 0)
    {
        // TODO
    }
}
}
