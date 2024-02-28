#include "VST3PlugFrame.hpp"

#include "audio/plugin/VST3PluginGUI.hpp"

namespace YADAW::Audio::Plugin
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
        Steinberg::ViewRect oldSize;
        if(auto result = view->getSize(&oldSize); result != Steinberg::kResultOk)
        {
            return result;
        }
        if(auto result = view->onSize(newSize); result != Steinberg::kResultOk)
        {
            view->onSize(&oldSize);
            return result;
        }
        auto& tweakedNewSize = oldSize;
        if(auto result = view->getSize(&tweakedNewSize); result != Steinberg::kResultOk)
        {
            return result;
        }
        gui_->disconnect();
        window->resize(tweakedNewSize.getWidth(), tweakedNewSize.getHeight());
        gui_->connect();
        return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
}
}
