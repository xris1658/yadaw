#include "VST3PlugFrame.hpp"

#include "audio/host/VST3RunLoop.hpp"
#include "audio/plugin/VST3PluginGUI.hpp"

#include <pluginterfaces/base/funknown.h>

#if __linux__
#include "VST3Host.hpp"
#endif

namespace YADAW::Audio::Host
{
VST3PlugFrame::VST3PlugFrame(YADAW::Audio::Plugin::VST3PluginGUI* gui):
    gui_(gui)
{}

VST3PlugFrame::~VST3PlugFrame()
{
}

Steinberg::tresult VST3PlugFrame::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown::iid, Steinberg::IPlugFrame)
    QUERY_INTERFACE(_iid, obj, Steinberg::IPlugFrame::iid, Steinberg::IPlugFrame)
#ifdef __linux__
    if(Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Linux::IRunLoop::iid))
    {
        *obj = &(YADAW::Audio::Host::VST3Host::instance().runLoop());
        return Steinberg::kResultOk;
    }
#endif
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 VST3PlugFrame::addRef()
{
    return 1;
}

Steinberg::uint32 VST3PlugFrame::release()
{
    return 1;
}

Steinberg::tresult VST3PlugFrame::resizeView(Steinberg::IPlugView* view, Steinberg::ViewRect* newSize)
{
    if(auto window = gui_->window())
    {
        if(resizeInitiatedFromPluginCallback_)
        {
#ifndef __APPLE__
            auto devicePixelRatio = window->devicePixelRatio();
#endif
            if(auto size = QSize(
#if __APPLE__
                newSize->getWidth(), newSize->getHeight()
#else
                newSize->getWidth() / devicePixelRatio,
                newSize->getHeight() / devicePixelRatio
#endif
            ); resizeInitiatedFromPluginCallback_(*(gui_->window()), size))
            {
                gui_->resize(size);
                return Steinberg::kResultOk;
            }
            else
            {
                return Steinberg::kResultFalse;
            }
        }
    }
    return Steinberg::kInvalidArgument;
}

void VST3PlugFrame::setResizeInitiatedFromPluginCallback(std::function<ResizeInitiatedFromPluginCallback>&& callback)
{
    resizeInitiatedFromPluginCallback_ = std::move(callback);
}

void VST3PlugFrame::resetResizeInitiatedFromPluginCallback()
{
    resizeInitiatedFromPluginCallback_ = {};
}
}
