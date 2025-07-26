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
    // https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/Workflow+Diagrams/Resize+View+Call+Sequence.html#initiated-from-plug-in
    if(auto window = gui_->window())
    {
        gui_->resizeViewCalled();
        auto devicePixelRatio = window->devicePixelRatio();
        Steinberg::ViewRect oldSize;
        if(auto result = view->getSize(&oldSize); result != Steinberg::kResultOk)
        {
            return result;
        }
        window->resize(
            std::round(newSize->getWidth() / devicePixelRatio),
            std::round(newSize->getHeight() / devicePixelRatio)
        );
        if(auto result = view->onSize(newSize); result != Steinberg::kResultOk)
        {
            view->onSize(&oldSize);
            return result;
        }
        view->getSize(newSize);
        return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
}
}
