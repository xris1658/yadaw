#include "PluginWindow.hpp"

#include "native/Window.hpp"

namespace YADAW::Audio::Plugin
{
using YADAW::UI::ResizeEventFilter;

PluginWindow::PluginWindow():
    QWindow(),
    pluginFrame_(this),
    resizeEventFilter_(*this)
{
    pluginFrame_.show();
    QObject::connect(
        &resizeEventFilter_, &YADAW::UI::ResizeEventFilter::aboutToResize,
        this, &PluginWindow::onAboutToResize
    );
    QObject::connect(
        &resizeEventFilter_, &YADAW::UI::ResizeEventFilter::resized,
        this, &PluginWindow::onResized
    );
}

PluginWindow::~PluginWindow()
{
    resetGUI();
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

void PluginWindow::setGUI(YADAW::Audio::Plugin::IPluginGUI& pluginGUI)
{
    resizeOps_ = Repositioning;
    if(pluginGUI_)
    {
        pluginGUI_->detachWithWindow();
    }
    pluginGUI.attachToWindow(&pluginFrame_);
    auto size = pluginFrame_.size().grownBy(QMargins(0, 0, 0, pluginFrame_.y()));
    resize(size);
    pluginGUI_ = &pluginGUI;
    resizeOps_ = 0;
}

void PluginWindow::resetGUI()
{
    if(pluginGUI_)
    {
        pluginGUI_->detachWithWindow();
    }
    pluginGUI_ = nullptr;
}

void PluginWindow::resizeFromPlugin(const QSize& size)
{
    resizeOps_ |= ResizeOp::ResizingFromPlugin;
    resize(size.width(), size.height() + pluginFrame_.y());
    if(topBar_)
    {
        topBar_->setWidth(size.width());
    }
    resizeOps_ ^= ResizeOp::ResizingFromPlugin;
}

void PluginWindow::onAboutToResize(YADAW::UI::ResizeEventFilter::DragPosition dragPosition, QRect* rect)
{
    if(resizeOps_ == 0)
    {
        if((ResizeEventFilter::getNativeSupportFlags()
            & ResizeEventFilter::FeatureSupportFlag::SupportsAdjustOnAboutToResize))
        {
            if(YADAW::Native::isWindowResizableByUser(*this))
            {
                auto size = rect->size();
                if(pluginGUI_ && pluginGUI_->adjustSize(size))
                {
                    YADAW::UI::ResizeEventFilter::adjustRect(*rect, dragPosition, size);
                }
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
                frameSize.height() - pluginFrame_.y()
            );
            pluginFrame_.resize(frameSize);
            if((resizeOps_ & ResizeOp::Repositioning) && pluginGUI_)
            {
                pluginGUI_->resize(frameSize);
            }
        }
    }
}
}
