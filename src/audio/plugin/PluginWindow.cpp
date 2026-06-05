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
    QObject::connect(
        this, &QWindow::visibleChanged, this, &PluginWindow::onVisibleChanged
    );
    QObject::connect(
        this, &QWindow::windowTitleChanged, this, &PluginWindow::onWindowTitleChanged
    );
    setFlags(
        Qt::WindowType::Dialog
      | Qt::WindowType::CustomizeWindowHint
      | Qt::WindowType::WindowTitleHint
      | Qt::WindowType::WindowCloseButtonHint
    );
}

PluginWindow::~PluginWindow()
{
    canClose_ = true;
    resetGUI();
    close();
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
        QObject::disconnect(topBar_, &QWindow::heightChanged, this, nullptr);
        resizeOps_ |= ResizeOp::Repositioning;
        pluginFrame_.setPosition(0, 0);
        setHeight(height() - topBar_->height());
        topBar_ = bar;
        resizeOps_ ^= ResizeOp::Repositioning;
    }
    else if(bar != nullptr && bar != topBar_)
    {
        if(topBar_)
        {
            QObject::disconnect(topBar_, &QWindow::heightChanged, this, nullptr);
        }
        QObject::connect(bar, &QWindow::heightChanged, this, &PluginWindow::onTopBarHeightChanged);
        bar->setParent(this);
        bar->setVisible(isVisible());
        bar->setWidth(width());
        resizeOps_ |= ResizeOp::Repositioning;
        pluginFrame_.setPosition(0, bar->height());
        setHeight(height() + bar->height());
        topBar_ = bar;
        resizeOps_ ^= ResizeOp::Repositioning;
        updateTopBarTitle();
    }
}

void PluginWindow::setGUI(YADAW::Audio::Plugin::IPluginGUI& pluginGUI)
{
    resizeOps_ ^= Repositioning;
    if(pluginGUI_)
    {
        pluginGUI_->detachWithWindow();
    }
    pluginGUI.attachToWindow(&pluginFrame_);
    auto size = pluginFrame_.size().grownBy(QMargins(0, 0, 0, pluginFrame_.y()));
    resize(size);
    YADAW::Native::setWindowResizableByUser(*this, pluginGUI.resizableByUser());
    pluginGUI_ = &pluginGUI;
    resizeOps_ ^= Repositioning;
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
    resizeOps_ ^= ResizeOp::ResizingFromPlugin;
}

bool PluginWindow::canClose() const
{
    return canClose_;
}

void PluginWindow::setCanClose(bool arg)
{
    canClose_ = arg;
}

void PluginWindow::closeEvent(QCloseEvent* closeEvent)
{
    if(canClose_)
    {
        closeEvent->accept();
    }
    else
    {
        closeEvent->ignore();
        hide();
    }
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
                auto& height = size.rheight();
                height -= pluginFrame_.y();
                if(pluginGUI_ && pluginGUI_->adjustSize(size))
                {
                    height += pluginFrame_.y();
                    YADAW::UI::ResizeEventFilter::adjustRect(*rect, dragPosition, size);
                }
            }
        }
    }
}

void PluginWindow::onResized(QRect rect)
{
    if((resizeOps_ & ResizeOp::Repositioning) == 0)
    {
        if((ResizeEventFilter::getNativeSupportFlags()
            & ResizeEventFilter::FeatureSupportFlag::SupportsAdjustOnAboutToResize))
        {
            auto frameSize = rect.size();
            frameSize.setHeight(
                frameSize.height() - pluginFrame_.y()
            );
            pluginFrame_.resize(frameSize);
            if(pluginGUI_)
            {
                pluginGUI_->resize(frameSize);
            }
            if(topBar_)
            {
                topBar_->setWidth(frameSize.width());
            }
        }
    }
}

void PluginWindow::onTopBarHeightChanged(int height)
{
    resizeOps_ ^= ResizeOp::Repositioning;
    setHeight(height + pluginFrame_.height());
    resizeOps_ ^= ResizeOp::Repositioning;
}

void PluginWindow::onVisibleChanged(bool visible)
{
    pluginFrame_.setVisible(visible);
    if(topBar_)
    {
        topBar_->setVisible(visible);
    }
}

void PluginWindow::onWindowTitleChanged(const QString& title)
{
    auto frameTitle = QString(title); frameTitle.append(" plugin frame");
    pluginFrame_.setTitle(frameTitle);
    if(topBar_)
    {
        updateTopBarTitle();
    }
}

void PluginWindow::updateTopBarTitle()
{
    auto topBarTitle = QString(title()); topBarTitle.append(" top bar");
    topBar_->setTitle(topBarTitle);
}
}
