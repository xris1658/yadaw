#ifndef YADAW_SRC_AUDIO_HOST_CLAPHOST
#define YADAW_SRC_AUDIO_HOST_CLAPHOST

#include <clap/host.h>
#include <clap/ext/gui.h>
#include <clap/ext/latency.h>
#include <clap/ext/params.h>
#include <clap/ext/thread-check.h>

#include <thread>

namespace YADAW::Audio::Plugin
{
class CLAPPlugin;
}

namespace YADAW::Audio::Host
{
class CLAPHost
{
public:
    CLAPHost(YADAW::Audio::Plugin::CLAPPlugin* plugin);
public:
    const clap_host* host();
protected: // clap_host functions
    static const void* getExtension(const clap_host* host, const char* extensionId);
private:
    static void requestRestart(const clap_host* host);
    static void requestProcess(const clap_host* host);
    static void requestCallback(const clap_host* host);
private: // clap_hsot_gui functions
    static void resizeHintsChanged(const clap_host* host);
    static bool requestResize(const clap_host* host, std::uint32_t width, std::uint32_t height);
    static bool requestShow(const clap_host* host);
    static bool requestHide(const clap_host* host);
    static void closed(const clap_host* host, bool wasDestroyed);
private: // clap_host_latency functions
    static void changed(const clap_host* host);
private: // clap_host_params functions
    static void rescan(const clap_host* host, clap_param_rescan_flags flags);
    static void clear(const clap_host* host, clap_id paramId, clap_param_clear_flags flags);
    static void requestFlush(const clap_host* host);
private: // clap_host_thread_check functions
    static bool isMainThread(const clap_host* host);
    static bool isAudioThread(const clap_host* host);
private: // clap_host implementations
    const void* doGetExtension(const char* extensionId);
    void doRequestRestart();
    void doRequestProcess();
    void doRequestCallback();
    void doResizeHintsChanged();
    bool doRequestResize(std::uint32_t width, std::uint32_t height);
    bool doRequestShow();
    bool doRequestHide();
    void doClosed(bool wasDestroyed);
    void doChanged();
    void doRescan(clap_param_rescan_flags flags);
    void doClear(clap_id paramId, clap_param_clear_flags flags);
    void doRequestFlush();
public:
    YADAW::Audio::Plugin::CLAPPlugin* plugin();
    void setMainThreadId(std::thread::id mainThreadId);
    void setAudioThreadId(std::thread::id audioThreadId);
private:
    static std::thread::id mainThreadId_;
    static std::thread::id audioThreadId_;
    YADAW::Audio::Plugin::CLAPPlugin* plugin_;
    clap_host host_;
    clap_host_gui gui_;
    clap_host_latency latency_;
    clap_host_params params_;
    clap_host_thread_check threadCheck_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_CLAPHOST
