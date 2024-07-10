#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI

#include "IPluginGUI.hpp"

#include <clap/plugin.h>
#include <clap/ext/gui.h>

#include <QMetaObject>
#include <QWindow>

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
    const QWindow* window() const override;
    QWindow* window() override;
    bool detachWithWindow() override;
    bool resizableByUser() const override;
    void requestResizeCalled();
public:
    void fetchResizeHints();
    clap_gui_resize_hints resizeHints() const;
private:
    void connect();
    void disconnect();
    const clap_plugin_gui* gui();
    void onWindowSizeChanged();
public:

private:
    const clap_plugin* plugin_ = nullptr;
    const clap_plugin_gui* gui_ = nullptr;
    QWindow* window_ = nullptr;
    clap_window clapWindow_ = {};
    clap_gui_resize_hints resizeHints_;
    QMetaObject::Connection connections_[2];
    bool requestResizeCalled_ = false;
};

}

#endif // YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINGUI
