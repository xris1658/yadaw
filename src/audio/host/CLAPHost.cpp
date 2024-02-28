#include "CLAPHost.hpp"

#include "audio/plugin/CLAPPlugin.hpp"

namespace YADAW::Audio::Host
{
CLAPHost* getHost(const clap_host* host)
{
    return reinterpret_cast<CLAPHost*>(host->host_data);
}

std::thread::id CLAPHost::mainThreadId_ = {};
std::thread::id CLAPHost::audioThreadId_ = {};

CLAPHost::CLAPHost(YADAW::Audio::Plugin::CLAPPlugin& plugin):
    plugin_(&plugin),
    latencyChanged_([]() {}),
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
    },
    threadCheck_
    {
        &isMainThread,
        &isAudioThread
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

bool CLAPHost::isMainThread(const clap_host* host)
{
    return std::this_thread::get_id() == mainThreadId_;
}

bool CLAPHost::isAudioThread(const clap_host* host)
{
    return std::this_thread::get_id() == audioThreadId_;
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
    if(std::strcmp(extensionId,CLAP_EXT_THREAD_CHECK) == 0)
    {
        return &threadCheck_;
    }
    return nullptr;
}

void CLAPHost::doRequestRestart()
{
    auto processing = plugin_->status() == YADAW::Audio::Plugin::IAudioPlugin::Status::Processing;
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
    if(plugin_->status() == YADAW::Audio::Plugin::IAudioPlugin::Status::Initialized)
    {
        plugin_->activate();
    }
    if(plugin_->status() == YADAW::Audio::Plugin::IAudioPlugin::Status::Activated)
    {
        plugin_->startProcessing();
    }
}

void CLAPHost::doRequestCallback()
{
    if(std::this_thread::get_id() != mainThreadId_)
    {
        // FIXME: If this function is not called on main thread, schedule a separate call
        return;
    }
    plugin_->calledOnMainThread();
}

void CLAPHost::doResizeHintsChanged()
{
    static_cast<YADAW::Audio::Plugin::CLAPPluginGUI*>(plugin_->pluginGUI())->fetchResizeHints();
}

bool CLAPHost::doRequestResize(std::uint32_t width, std::uint32_t height)
{
    auto window = plugin_->pluginGUI()->window();
    window->resize(width, height);
    return true;
}

bool CLAPHost::doRequestShow()
{
    plugin_->pluginGUI()->window()->show();
    return false;
}

bool CLAPHost::doRequestHide()
{
    plugin_->pluginGUI()->window()->hide();
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
    latencyChanged_();
}

void CLAPHost::doRescan(clap_param_rescan_flags flags)
{
    if(flags & CLAP_PARAM_RESCAN_ALL)
    {
        plugin_->refreshAllParameter();
    }
    else
    {
        if(flags & CLAP_PARAM_RESCAN_VALUES)
        {
            parameterValueChanged_();
        }
        if(flags & CLAP_PARAM_RESCAN_TEXT)
        {
            parameterTextChanged_();
        }
        if(flags & CLAP_PARAM_RESCAN_INFO)
        {
            plugin_->pluginParameter()->refreshParameterInfo();
            parameterInfoChanged_();
        }
    }
}

void CLAPHost::doClear(clap_id paramId, clap_param_clear_flags flags)
{
    // TODO
}

void CLAPHost::doRequestFlush()
{
    // TODO
}

YADAW::Audio::Plugin::CLAPPlugin* CLAPHost::plugin()
{
    return plugin_;
}

void CLAPHost::setMainThreadId(std::thread::id mainThreadId)
{
    mainThreadId_ = mainThreadId;
}

void CLAPHost::setAudioThreadId(std::thread::id audioThreadId)
{
    audioThreadId_ = audioThreadId;
}

void CLAPHost::latencyChanged(std::function<void()>&& callback)
{
    latencyChanged_ = std::move(callback);
}

void CLAPHost::parameterValueChanged(std::function<void()>&& callback)
{
    parameterValueChanged_ = std::move(callback);
}

void CLAPHost::parameterTextChanged(std::function<void()>&& callback)
{
    parameterTextChanged_ = std::move(callback);
}

void CLAPHost::parameterInfoChanged(std::function<void()>&& callback)
{
    parameterInfoChanged_ = std::move(callback);
}
}