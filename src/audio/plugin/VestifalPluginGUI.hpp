#ifndef YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINGUI

#include "audio/plugin/vestifal/Vestifal.h"
#include "audio/plugin/IPluginGUI.hpp"

namespace YADAW::Audio::Plugin
{
class VestifalPluginGUI: public IPluginGUI
{
public:
    VestifalPluginGUI(AEffect& effect);
    VestifalPluginGUI(const VestifalPluginGUI&) = delete;
    ~VestifalPluginGUI() override;
public:
    bool attachToWindow(QWindow* window) override;
    QWindow* window() override;
    const QWindow* window() const override;
    bool detachWithWindow() override;
private:
    AEffect* effect_;
    QWindow* window_ = nullptr;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINGUI
