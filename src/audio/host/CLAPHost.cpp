#include "CLAPHost.hpp"

#include "audio/plugin/CLAPPlugin.hpp"

namespace YADAW::Audio::Host
{
CLAPHost* getHost(const clap_host* host)
{
    return reinterpret_cast<CLAPHost*>(host->host_data);
}
CLAPHost::CLAPHost(YADAW::Audio::Plugin::CLAPPlugin* plugin):
    plugin_(plugin),
    host_
    {
        {
            CLAP_VERSION_MAJOR,
            CLAP_VERSION_MINOR,
            CLAP_VERSION_REVISION
        },
        this,
        "YADAW",
        "xris1658",
        "https://github.com/xris1658/yadaw",
        "0.0.1",
        &getExtension,
        &requestRestart,
        &requestProcess,
        &requestCallback
    },
    gui_
    {
        &resizeHintsChanged,
        &requestResize,
        &requestShow,
        &requestHide,
        &closed
    },
    latency_
    {
        &changed
    },
    params_
    {
        &rescan,
        &clear,
        &requestFlush
    }
{
}

const clap_host* CLAPHost::host()
{
    return &host_;
}

const void* CLAPHost::getExtension(const clap_host* host, const char* extensionId)
{
    return getHost(host)->doGetExtension(extensionId);
}

void CLAPHost::requestRestart(const clap_host* host)
{
    return getHost(host)->doRequestRestart();
}

void CLAPHost::requestProcess(const clap_host* host)
{
    return getHost(host)->doRequestProcess();
}

void CLAPHost::requestCallback(const clap_host* host)
{
    return getHost(host)->doRequestCallback();
}

void CLAPHost::resizeHintsChanged(const clap_host* host)
{
    return getHost(host)->doResizeHintsChanged();
}

bool CLAPHost::requestResize(const clap_host* host, std::uint32_t width, std::uint32_t height)
{
    return getHost(host)->doRequestResize(width, height);
}

bool CLAPHost::requestShow(const clap_host* host)
{
    return getHost(host)->doRequestShow();
}

bool CLAPHost::requestHide(const clap_host* host)
{
    return getHost(host)->doRequestHide();
}

void CLAPHost::closed(const clap_host* host, bool wasDestroyed)
{
    return getHost(host)->doClosed(wasDestroyed);
}

void CLAPHost::changed(const clap_host* host)
{
    return getHost(host)->doChanged();
}

void CLAPHost::rescan(const clap_host* host, clap_param_rescan_flags flags)
{
    return getHost(host)->doRescan(flags);
}

void CLAPHost::clear(const clap_host* host, clap_id paramId, clap_param_clear_flags flags)
{
    return getHost(host)->doClear(paramId, flags);
}

void CLAPHost::requestFlush(const clap_host* host)
{
    return getHost(host)->doRequestFlush();
}

const void* CLAPHost::doGetExtension(const char* extensionId)
{
    if(std::strcmp(extensionId, CLAP_EXT_GUI) == 0)
    {
        return &gui_;
    }
    if(std::strcmp(extensionId, CLAP_EXT_LATENCY) == 0)
    {
        return &latency_;
    }
    if(std::strcmp(extensionId, CLAP_EXT_PARAMS) == 0)
    {
        return &params_;
    }
    return nullptr;
}

void CLAPHost::doRequestRestart()
{
    auto processing = plugin_->status() == YADAW::Audio::Plugin::IPlugin::Status::Processing;
    if(processing)
    {
        plugin_->stopProcessing();
    }
    plugin_->deactivate();
    plugin_->activate();
    if(processing)
    {
        plugin_->startProcessing();
    }
}

void CLAPHost::doRequestProcess()
{
    if(plugin_->status() == YADAW::Audio::Plugin::IPlugin::Status::Initialized)
    {
        plugin_->activate();
    }
    if(plugin_->status() == YADAW::Audio::Plugin::IPlugin::Status::Activated)
    {
        plugin_->startProcessing();
    }
}

void CLAPHost::doRequestCallback()
{
    plugin_->calledOnMainThread();
}

void CLAPHost::doResizeHintsChanged()
{
    static_cast<YADAW::Audio::Plugin::CLAPPluginGUI*>(plugin_->gui())->fetchResizeHints();
}

bool CLAPHost::doRequestResize(std::uint32_t width, std::uint32_t height)
{
    auto window = plugin_->gui()->window();
    window->setWidth(width);
    window->setHeight(height);
    return true;
}

bool CLAPHost::doRequestShow()
{
    plugin_->gui()->window()->show();
    return false;
}

bool CLAPHost::doRequestHide()
{
    plugin_->gui()->window()->hide();
    return true;
}

void CLAPHost::doClosed(bool wasDestroyed)
{
    if(wasDestroyed)
    {
        // TODO: call clap_plugin_gui->destroy()
    }
}

void CLAPHost::doChanged()
{
    // TODO
}

void CLAPHost::doRescan(clap_param_rescan_flags flags)
{
    // TODO
}

void CLAPHost::doClear(clap_id paramId, clap_param_clear_flags flags)
{
    // TODO
}

void CLAPHost::doRequestFlush()
{
    // TODO
}
}