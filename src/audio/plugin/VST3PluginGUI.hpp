#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI

#include "audio/plugin/IPluginGUI.hpp"

#include <pluginterfaces/gui/iplugview.h>

namespace YADAW::Audio::Plugin
{
class VST3PluginGUI: public YADAW::Audio::Plugin::IPluginGUI
{
public:
    VST3PluginGUI(Steinberg::IPlugView* plugView);
    VST3PluginGUI(const VST3PluginGUI&) = delete;
    VST3PluginGUI(VST3PluginGUI&& rhs) noexcept = default;
    VST3PluginGUI& operator=(const VST3PluginGUI&) = delete;
    VST3PluginGUI& operator=(VST3PluginGUI&&) noexcept = default;
    ~VST3PluginGUI() override;
public:
    bool attachToWindow(QWindow* window) override;
    QWindow* window() override;
    bool detachWithWindow() override;
public:
    Steinberg::IPlugView* plugView();
private:
    Steinberg::IPlugView* plugView_ = nullptr;
    QWindow* window_ = nullptr;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
