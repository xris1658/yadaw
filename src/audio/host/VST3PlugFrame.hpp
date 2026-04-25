#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME

#include <pluginterfaces/gui/iplugview.h>

#include <QSize>

#include <functional>

namespace YADAW::Audio::Plugin
{
class VST3PluginGUI;
}

namespace YADAW::Audio::Host
{
class VST3PlugFrame final: public Steinberg::IPlugFrame
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
    using ResizeInitiatedFromPluginCallback = bool(const QSize& size);
    void setResizeInitiatedFromPluginCallback(std::function<ResizeInitiatedFromPluginCallback>&& callback);
    void resetResizeInitiatedFromPluginCallback();
private:
    YADAW::Audio::Plugin::VST3PluginGUI* gui_;
    std::function<ResizeInitiatedFromPluginCallback> resizeInitiatedFromPluginCallback_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VST3PLUGFRAME
