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

bool VST3PluginGUI::usePhysicalPixelSize() const // TODO
{
#if _WIN32 || __linux__
    return true;
#endif
    return false;
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
    if(!ret)
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

QSize VST3PluginGUI::size() const
{
    if(plugView_)
    {
        Steinberg::ViewRect rect; plugView_->getSize(&rect);
        return QSize(rect.getWidth(), rect.getHeight());
    }
    return QSize();
}

bool VST3PluginGUI::adjustSize(QSize& size)
{
    if(plugView_)
    {
        Steinberg::ViewRect rect(0, 0,
            size.width(), size.height()
        );
        auto ret = plugView_->checkSizeConstraint(&rect);
        if(ret == Steinberg::kResultOk)
        {
            size = QSize(
                rect.getWidth(), rect.getHeight()
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
        Steinberg::ViewRect rect(0, 0,
            size.width(), size.height()
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
