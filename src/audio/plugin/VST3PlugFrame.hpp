#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME

#include <pluginterfaces/gui/iplugview.h>

namespace YADAW::Audio::Plugin
{
class VST3PluginGUI;

class VST3PlugFrame: public Steinberg::IPlugFrame
{
public:
    VST3PlugFrame(YADAW::Audio::Plugin::VST3PluginGUI* gui);
    VST3PlugFrame(const VST3PlugFrame&) = delete;
    VST3PlugFrame(VST3PlugFrame&&) = default;
    VST3PlugFrame& operator=(VST3PlugFrame&&) = default;
    ~VST3PlugFrame();
public:
    Steinberg::tresult queryInterface(const Steinberg::TUID iid, void** obj) override;
    Steinberg::uint32 addRef() override;
    Steinberg::uint32 release() override;
public:
    Steinberg::tresult resizeView(Steinberg::IPlugView* view,
        Steinberg::ViewRect* newSize) override;
private:
    YADAW::Audio::Plugin::VST3PluginGUI* gui_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME
