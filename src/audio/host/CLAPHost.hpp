#ifndef YADAW_SRC_AUDIO_HOST_CLAPHOST
#define YADAW_SRC_AUDIO_HOST_CLAPHOST

#include <clap/host.h>
#include <clap/ext/gui.h>
#include <clap/ext/latency.h>
#include <clap/ext/params.h>
#include <clap/ext/thread-check.h>
#include <clap/ext/timer-support.h>
#if __linux__
#include <clap/ext/posix-fd-support.h>
#endif

#include <functional>
#include <thread>
#include <set>

namespace YADAW::Audio::Plugin
{
class CLAPPlugin;
}

namespace YADAW::Audio::Host
{
class CLAPHost
{
    friend class YADAW::Audio::Plugin::CLAPPlugin;
public:
    using LatencyChangedCallback = void(YADAW::Audio::Plugin::CLAPPlugin&);
    using ParameterValueChangedCallback = void(YADAW::Audio::Plugin::CLAPPlugin&);
    using ParameterTextChangedCallback = void(YADAW::Audio::Plugin::CLAPPlugin&);
    using ParameterInfoChangedCallback = void(YADAW::Audio::Plugin::CLAPPlugin&);
public:
    CLAPHost(YADAW::Audio::Plugin::CLAPPlugin& plugin);
public:
    const clap_host* host();
protected: // clap_host functions
    static const void* getExtension(const clap_host* host, const char* extensionId);
private:
    static void requestRestart(const clap_host* host);
    static void requestProcess(const clap_host* host);
    static void requestCallback(const clap_host* host);
private: // clap_host_gui functions
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
private: // clap_host_timer_support functions
    static bool registerTimer(const clap_host* host, std::uint32_t milliseconds, clap_id* timerId);
    static bool unregisterTimer(const clap_host* host, clap_id timerId);
#if __linux__
private: // clap_host_posix_fd_support functions
    static bool registerFD(const clap_host* host, int fd, clap_posix_fd_flags_t flags);
    static bool modifyFD(const clap_host* host, int fd, clap_posix_fd_flags_t flags);
    static bool unregisterFD(const clap_host* host, int fd);
#endif
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
    void doLatencyChanged();
    void doParameterRescan(clap_param_rescan_flags flags);
    void doParameterClear(clap_id paramId, clap_param_clear_flags flags);
    void doParameterRequestFlush();
    bool doRegisterTimer(std::uint32_t milliseconds, clap_id* timerId);
    bool doUnregisterTimer(clap_id timerId);
#if __linux__
    bool doRegisterFD(int fd, clap_posix_fd_flags_t flags);
    bool doModifyFD(int fd, clap_posix_fd_flags_t flags);
    bool doUnregisterFD(int fd);
#endif
public:
    YADAW::Audio::Plugin::CLAPPlugin* plugin();
    static void setMainThreadId(std::thread::id mainThreadId);
    static void setAudioThreadId(std::thread::id audioThreadId);
    static void unsetAudioThreadId(std::thread::id audioThreadId);
    static void clearAudioThreadId();
    void setLatencyChangedCallback(LatencyChangedCallback* callback);
    void setParameterValueChangedCallback(ParameterValueChangedCallback* callback);
    void setParameterTextChangedCallback(ParameterTextChangedCallback* callback);
    void setParameterInfoChangedCallback(ParameterInfoChangedCallback* callback);
    void resetLatencyChangedCallback();
    void resetParameterValueChangedCallback();
    void resetParameterTextChangedCallback();
    void resetParameterInfoChangedCallback();
private:
    static std::thread::id mainThreadId_;
    static std::set<std::thread::id> audioThreadIds_;
    YADAW::Audio::Plugin::CLAPPlugin* plugin_;
    LatencyChangedCallback* latencyChangedCallback_;
    ParameterValueChangedCallback* parameterValueChangedCallback_;
    ParameterTextChangedCallback* parameterTextChangedCallback_;
    ParameterInfoChangedCallback* parameterInfoChangedCallback_;
    clap_host host_;
    clap_host_gui gui_;
    clap_host_latency latency_;
    clap_host_params params_;
    clap_host_thread_check threadCheck_;
    clap_host_timer_support timerSupport_;
#if __linux__
    clap_host_posix_fd_support posixFDSupport_;
#endif
};
}

#endif // YADAW_SRC_AUDIO_HOST_CLAPHOST
