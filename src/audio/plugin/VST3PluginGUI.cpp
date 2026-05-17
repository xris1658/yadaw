#include "VST3PluginGUI.hpp"

#include "audio/util/VST3Helper.hpp"
#include "native/VST3Native.hpp"

namespace YADAW::Audio::Plugin
{
VST3PluginGUI::VST3PluginGUI(Steinberg::IPlugView* plugView):
    plugView_(plugView),
    window_(nullptr),
    frame_(this)
{
    plugView_->queryInterface(
        Steinberg::IPlugViewContentScaleSupport::iid,
        reinterpret_cast<void**>(&plugViewContentScaleSupport_)
    );
    plugView_->setFrame(&frame_);
}

VST3PluginGUI::~VST3PluginGUI()
{
    if(window_)
    {
        VST3PluginGUI::detachWithWindow();
    }
    if(plugView_)
    {
        releasePointer(plugViewContentScaleSupport_);
        plugView_->setFrame(nullptr);
        releasePointer(plugView_);
    }
}

bool VST3PluginGUI::attachToWindow(QWindow* window)
{
    if(window_)
    {
        return false;
    }
    window_ = window;
#ifndef __APPLE__
    auto devicePixelRatio = window->devicePixelRatio();
    if(plugViewContentScaleSupport_)
    {
        plugViewContentScaleSupport_->setContentScaleFactor(devicePixelRatio);
    }
#endif
    auto ret = plugView_->attached(
        reinterpret_cast<void*>(window->winId()),
        YADAW::Native::ViewType
    ) == Steinberg::kResultOk;
    if(ret)
    {
        Steinberg::ViewRect rect;
        plugView_->getSize(&rect);
        window_->resize(
#if __APPLE__
            rect.getWidth(), rect.getHeight()
#else
            rect.getWidth() / devicePixelRatio,
            rect.getHeight() / devicePixelRatio
#endif
        );
    }
    else
    {
        window_ = nullptr;
    }
    return ret;
}

QWindow* VST3PluginGUI::window()
{
    return window_;
}

const QWindow* VST3PluginGUI::window() const
{
    return window_;
}

bool VST3PluginGUI::detachWithWindow()
{
    if(!window_)
    {
        return true;
    }
    auto detachResult = plugView_->removed();
    if(detachResult == Steinberg::kResultOk)
    {
        window_ = nullptr;
    }
    return detachResult == Steinberg::kResultOk;
}

bool VST3PluginGUI::resizableByUser() const
{
    if(plugView_)
    {
        return plugView_->canResize() == Steinberg::kResultTrue;
    }
    return false;
}

bool VST3PluginGUI::adjustSize(QSize& size)
{
    if(plugView_)
    {
#ifndef __APPLE__
        auto devicePixelRatio = window_->devicePixelRatio();
#endif
        Steinberg::ViewRect rect(0, 0,
#if __APPLE__
            size.width(), size.height()
#else
            size.width() * devicePixelRatio,
            size.height() * devicePixelRatio
#endif
        );
        auto ret = plugView_->checkSizeConstraint(&rect);
        if(ret == Steinberg::kResultOk)
        {
            size = QSize(
#if __APPLE__
                rect.getWidth(), rect.getHeight()
#else
                rect.getWidth() / devicePixelRatio,
                rect.getHeight() / devicePixelRatio
#endif
            );
            return true;
        }
    }
    return false;
}

bool VST3PluginGUI::resize(const QSize& size)
{
    if(plugView_)
    {
#ifndef __APPLE__
        auto devicePixelRatio = window_->devicePixelRatio();
#endif
        Steinberg::ViewRect rect(0, 0,
#if __APPLE__
            size.width(), size.height()
#else
            size.width() * devicePixelRatio,
            size.height() * devicePixelRatio
#endif
        );
        return plugView_->onSize(&rect) == Steinberg::kResultOk;
    }
    return false;
}

YADAW::Audio::Host::VST3PlugFrame& VST3PluginGUI::frame()
{
    return frame_;
}
}
