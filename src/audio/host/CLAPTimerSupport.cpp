#include "CLAPTimerSupport.hpp"

#include <QCoreApplication>

namespace YADAW::Audio::Host
{
CLAPTimerSupport::CLAPTimerSupport():
    timerCallbacks_(),
    mainThreadContext_(QCoreApplication::instance())
{}

CLAPTimerSupport& CLAPTimerSupport::instance()
{
    static CLAPTimerSupport ret;
    return ret;
}

CLAPTimerSupport::~CLAPTimerSupport()
{
    for(auto& [plugin, timers]: timerCallbacks_)
    {
        for(auto& [id, timerStorage]: timers)
        {
            auto timer = YADAW::Util::AlignHelper<QTimer>::fromAligned(&timerStorage);
            timer->~QTimer();
        }
        timers.clear();
    }
    timerCallbacks_.clear();
}

bool CLAPTimerSupport::registerTimer(const clap_plugin_t* plugin,
    const clap_plugin_timer_support_t* timerSupport,
    clap_id* id, std::uint32_t millisecond)
{
    if(*id == CLAP_INVALID_ID)
    {
        return false;
    }
    auto& timers = timerCallbacks_[plugin];
    auto it = timers.find(*id);
    if(it == timers.end())
    {
        auto it2 = timers.emplace(*id, YADAW::Util::AlignedStorage<QTimer>()).first;
        auto* timer = new(&(it2->second)) QTimer();
        timer->setInterval(millisecond);
        timer->setSingleShot(false);
        timer->setTimerType(Qt::TimerType::PreciseTimer);
        timer->callOnTimeout(
            mainThreadContext_,
            [plugin, timerSupport, timerId = *id]()
            {
                timerSupport->on_timer(plugin, timerId);
            }
        );
        timer->start();
        return true;
    }
    else
    {
        auto timer = YADAW::Util::AlignHelper<QTimer>::fromAligned(&(it->second));
        timer->stop();
        timer->disconnect(mainThreadContext_);
        timer->setInterval(millisecond);
        timer->setSingleShot(false);
        timer->setTimerType(Qt::TimerType::PreciseTimer);
        timer->callOnTimeout(
            mainThreadContext_,
            [plugin, timerSupport, timerId = *id]()
            {
                timerSupport->on_timer(plugin, timerId);
            }
        );
        timer->start();
        return true;
    }
}

bool CLAPTimerSupport::unregisterTimer(const clap_plugin_t* plugin,
    const clap_plugin_timer_support_t* timerSupport, clap_id id)
{
    auto it = timerCallbacks_.find(plugin);
    if(it != timerCallbacks_.end())
    {
        auto& timers = it->second;
        auto it2 = timers.find(id);
        if(it2 != timers.end())
        {
            auto timer = YADAW::Util::AlignHelper<QTimer>::fromAligned(&(it2->second));
            timer->stop();
            timer->~QTimer();
            timers.erase(it2);
            return true;
        }
    }
    return false;
}
}
