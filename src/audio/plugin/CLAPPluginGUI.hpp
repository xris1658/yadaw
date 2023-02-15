#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI

#include "IPluginGUI.hpp"

#include <clap/plugin.h>
#include <clap/ext/gui.h>

namespace YADAW::Audio::Plugin
{
class CLAPPluginGUI: public IPluginGUI
{
public:
    CLAPPluginGUI(const clap_plugin* plugin, const clap_plugin_gui* gui);
    CLAPPluginGUI(const CLAPPluginGUI&) = delete;
    CLAPPluginGUI(CLAPPluginGUI&& rhs) noexcept = default;
    CLAPPluginGUI& operator=(const CLAPPluginGUI&) = delete;
    CLAPPluginGUI& operator=(CLAPPluginGUI&& rhs) noexcept = default;
    ~CLAPPluginGUI() override;
public:
    bool attachToWindow(QWindow* window) override;
    QWindow* window() override;
    bool detachWithWindow() override;
public:
    const clap_plugin_gui* gui();
private:
    const clap_plugin* plugin_ = nullptr;
    const clap_plugin_gui* gui_ = nullptr;
    QWindow* window_ = nullptr;
    clap_window clapWindow_ = {};
};

}

#endif //YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI
