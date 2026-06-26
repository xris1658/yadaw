#include "PluginWindow.hpp"

#include "native/Window.hpp"

#include <QCloseEvent>

namespace YADAW::Audio::Plugin
{
using YADAW::UI::WindowResizeEvents;

PluginWindow::PluginWindow():
    QWindow(),
    pluginFrame_(this),
    windowResizeEvents_(*this)
{
    pluginFrame_.show();
    QObject::connect(
        &windowResizeEvents_, &YADAW::UI::WindowResizeEvents::aboutToResize,
        this, &PluginWindow::onAboutToResize
    );
    QObject::connect(
        &windowResizeEvents_, &YADAW::UI::WindowResizeEvents::resized,
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
    if(pluginGUI_)
    {
        pluginGUI_->detachWithWindow();
    }
    pluginGUI_ = &pluginGUI;
    pluginGUI.attachToWindow(&pluginFrame_);
    // Some plugins might invoke `resizeFromPlugin` with the `attachToWindow` call above,
    // in which case we should resize all the child windows accordingly.
    resizeOps_ ^= Repositioning;
    auto pluginFrameSize = pluginGUI_->size();
    if(pluginGUI.usePhysicalPixelSize())
    {
        auto topBarHeight = topBar_? YADAW::Native::getPhysicalGeometry(*topBar_).height(): 0;
        YADAW::Native::setPhysicalGeometry(
            pluginFrame_, QRect(QPoint(0, topBarHeight), pluginFrameSize)
        );
        YADAW::Native::setPhysicalSize(
            *this, QSize(pluginFrameSize.width(), pluginFrameSize.height() + topBarHeight)
        );
    }
    else
    {
        auto topBarHeight = topBar_? topBar_->height(): 0;
        pluginFrame_.setGeometry(QRect(QPoint(0, topBarHeight), pluginFrameSize));
        resize(pluginFrameSize.width(), pluginFrameSize.height() + topBarHeight);
    }
    YADAW::Native::setWindowResizableByUser(*this, pluginGUI.resizableByUser());
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
    if(pluginGUI_->usePhysicalPixelSize())
    {
        auto pluginFrameGeometry = YADAW::Native::getPhysicalGeometry(pluginFrame_);
        YADAW::Native::setPhysicalSize(
            *this, QSize(size.width(), size.height() + pluginFrameGeometry.y())
        );
    }
    else
    {
        resize(size.width(), size.height() + pluginFrame_.y());
    }
    resizeOps_ ^= ResizeOp::ResizingFromPlugin;
}

void PluginWindow::closeEvent(QCloseEvent* closeEvent)
{
    bool canClose = true;
    aboutToClose(canClose);
    closeEvent->setAccepted(canClose);
}

void PluginWindow::onAboutToResize(YADAW::UI::WindowResizeEvents::DragPosition dragPosition, QRect* rect)
{
    using YADAW::UI::WindowResizeEvents;
    if(resizeOps_ == 0)
    {
        auto nativeSupportFlags = WindowResizeEvents::getNativeSupportFlags();
        if(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::SupportsAdjustOnAboutToResize)
        {
            if(pluginGUI_ && YADAW::Native::isWindowResizableByUser(*this))
            {
                auto size = rect->size();
                if(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::UsesPhysicalSize)
                {
                    if(pluginGUI_->usePhysicalPixelSize())
                    {
                        auto& height = size.rheight();
                        auto pluginFrameGeometry = YADAW::Native::getPhysicalGeometry(pluginFrame_);
                        height -= pluginFrameGeometry.y();
                        if(pluginGUI_->adjustSize(size))
                        {
                            height += pluginFrameGeometry.y();
                            WindowResizeEvents::adjustRect(*rect, dragPosition, size);
                        }
                    }
                    else
                    {
                        // TODO
                    }
                }
                else
                {
                    if(!pluginGUI_->usePhysicalPixelSize())
                    {
                        auto& height = size.rheight();
                        height -= pluginFrame_.y();
                        if(pluginGUI_->adjustSize(size))
                        {
                            height += pluginFrame_.y();
                            WindowResizeEvents::adjustRect(*rect, dragPosition, size);
                        }
                    }
                    else
                    {
                        // TODO
                    }
                }
            }
        }
    }
}

void PluginWindow::onResized(QRect rect)
{
    using YADAW::UI::WindowResizeEvents;
    if((resizeOps_ & ResizeOp::Repositioning) == 0)
    {
        auto nativeSupportFlags = WindowResizeEvents::getNativeSupportFlags();
        if(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::SupportsAdjustOnAboutToResize)
        {
            auto frameSize = rect.size();
            if(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::UsesPhysicalSize)
            {
                auto pluginFrameGeometry = YADAW::Native::getPhysicalGeometry(pluginFrame_);
                frameSize.setHeight(frameSize.height() - pluginFrameGeometry.y());
                YADAW::Native::setPhysicalSize(pluginFrame_, frameSize);
                if(pluginGUI_)
                {
                    if(pluginGUI_->usePhysicalPixelSize())
                    {
                        pluginGUI_->resize(frameSize);
                    }
                    else
                    {
                        // TODO
                    }
                }
                if(topBar_)
                {
                    auto topBarGeometry = YADAW::Native::getPhysicalGeometry(*topBar_);
                    YADAW::Native::setPhysicalSize(*topBar_, QSize(frameSize.width(), topBarGeometry.height()));
                }
            }
            else
            {
                if(!(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::UsesPhysicalSize))
                {
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
                else
                {
                    // TODO
                }
            }
        }
        else
        {
            auto size = rect.size();
            if(pluginGUI_ && YADAW::Native::isWindowResizableByUser(*this))
            {
                if(nativeSupportFlags & WindowResizeEvents::FeatureSupportFlag::UsesPhysicalSize)
                {
                    if(pluginGUI_->usePhysicalPixelSize())
                    {
                        auto oldGeo = YADAW::Native::getPhysicalGeometry(*this);
                        auto& height = size.rheight();
                        auto pluginFrameGeometry = YADAW::Native::getPhysicalGeometry(pluginFrame_);
                        height -= pluginFrameGeometry.y();
                        if(pluginGUI_->adjustSize(size))
                        {
                            pluginGUI_->resize(size);
                            YADAW::Native::setPhysicalSize(pluginFrame_, size);
                            if(topBar_)
                            {
                                auto topBarGeometry = YADAW::Native::getPhysicalGeometry(*topBar_);
                                YADAW::Native::setPhysicalSize(*topBar_, QSize(size.width(), topBarGeometry.height()));
                            }
                        }
                    }
                    else
                    {
                        // TODO
                    }
                }
                else
                {
                    if(!pluginGUI_->usePhysicalPixelSize())
                    {
                        auto& height = size.rheight();
                        height -= pluginFrame_.y();
                        if(pluginGUI_->adjustSize(size))
                        {
                            pluginGUI_->resize(size);
                            pluginFrame_.resize(size);
                            topBar_->setWidth(size.width());
                        }
                    }
                    else
                    {
                        // TODO
                    }
                }
            }
        }
    }
}

void PluginWindow::onTopBarHeightChanged(int height)
{
    using YADAW::UI::WindowResizeEvents;
    resizeOps_ ^= ResizeOp::Repositioning;
    if(WindowResizeEvents::getNativeSupportFlags() & WindowResizeEvents::FeatureSupportFlag::UsesPhysicalSize)
    {
        auto physicalHeight = YADAW::Native::getPhysicalGeometry(*topBar_).height();
        auto newSize = YADAW::Native::getPhysicalGeometry(*this).size();
        newSize.setHeight(newSize.height() + physicalHeight);
        YADAW::Native::setPhysicalSize(*this, newSize);
    }
    else
    {
        setHeight(height + pluginFrame_.height());
    }
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
