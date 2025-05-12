#ifndef YADAW_SRC_AUDIO_HOST_CLAPTIMERSUPPORT
#define YADAW_SRC_AUDIO_HOST_CLAPTIMERSUPPORT

#include "util/AlignHelper.hpp"

#include <clap/ext/timer-support.h>

#include <QObject>
#include <QThread>
#include <QTimer>

#include <map>
#include <type_traits>

namespace YADAW::Audio::Host
{
class CLAPTimerSupport
{
private:
    CLAPTimerSupport();
public:
    static CLAPTimerSupport& instance();
    ~CLAPTimerSupport();
public:
    bool registerTimer(const clap_plugin_t* plugin,
        const clap_plugin_timer_support_t* timerSupport,
        clap_id* id, std::uint32_t millisecond);
    bool unregisterTimer(const clap_plugin_t* plugin,
        const clap_plugin_timer_support_t* timerSupport,
        clap_id id);
private:
    std::map<
        const clap_plugin_t*,
        std::map<clap_id, YADAW::Util::AlignedStorage<QTimer>>
    > timerCallbacks_;
    QObject* mainThreadContext_;
};
}

#endif // YADAW_SRC_AUDIO_HOST_CLAPTIMERSUPPORT
