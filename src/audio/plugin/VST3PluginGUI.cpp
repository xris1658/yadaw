#include "VST3PluginGUI.hpp"

#include "native/VST3Native.hpp"

namespace YADAW::Audio::Plugin
{
VST3PluginGUI::VST3PluginGUI(Steinberg::IPlugView* plugView):
    plugView_(plugView)
{
}

VST3PluginGUI::~VST3PluginGUI()
{
    if(window_)
    {
        VST3PluginGUI::detachWithWindow();
    }
    if(plugView_)
    {
        plugView_->release();
        plugView_ = nullptr;
    }
}

bool VST3PluginGUI::attachToWindow(QWindow* window)
{
    if(window_)
    {
        return false;
    }
    auto attachResult = plugView_->attached(reinterpret_cast<void*>(window->winId()),
        YADAW::Native::ViewType);
    if(attachResult == Steinberg::kResultOk)
    {
        window_ = window;
    }
    return attachResult == Steinberg::kResultOk;
}

QWindow* VST3PluginGUI::window()
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
    if(detachResult)
    {
        window_ = nullptr;
    }
    return detachResult == Steinberg::kResultOk;
}

Steinberg::IPlugView* VST3PluginGUI::plugView()
{
    return plugView_;
}
}