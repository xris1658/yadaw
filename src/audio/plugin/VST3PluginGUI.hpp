#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI

#include "audio/plugin/IPluginGUI.hpp"
#include "audio/plugin/VST3PlugFrame.hpp"

#include <pluginterfaces/gui/iplugview.h>

#include <QMetaObject>
#include <QObject>
#include <QWindow>

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
    void connect();
    void disconnect();
private:
    void onWindowSizeChanged();
private:
    Steinberg::IPlugView* plugView_ = nullptr;
    QWindow* window_ = nullptr;
    YADAW::Audio::Plugin::VST3PlugFrame frame_;
    QMetaObject::Connection connections_[2];
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
