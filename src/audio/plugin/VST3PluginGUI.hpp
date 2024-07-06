#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI

#include "audio/host/VST3PlugFrame.hpp"
#include "audio/plugin/IPluginGUI.hpp"

#include <pluginterfaces/gui/iplugview.h>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>

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
    const QWindow* window() const override;
    bool detachWithWindow() override;
public:
    void connect();
    void disconnect();
    bool isConnected() const;
public:
    void resizeViewCalled();
private:
    void onWindowSizeChanged();
private:
    Steinberg::IPlugView* plugView_ = nullptr;
    Steinberg::IPlugViewContentScaleSupport* plugViewContentScaleSupport_ = nullptr;
    QWindow* window_ = nullptr;
    YADAW::Audio::Host::VST3PlugFrame frame_;
    QMetaObject::Connection connections_[2];
    bool isConnected_ = false;
    bool inCallback_ = false;
    bool resizeViewCalled_ = false;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINGUI
