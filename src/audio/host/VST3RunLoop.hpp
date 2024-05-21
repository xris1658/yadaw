#ifndef YADAW_SRC_AUDIO_HOST_VST3RUNLOOP
#define YADAW_SRC_AUDIO_HOST_VST3RUNLOOP
#include "util/AlignHelper.hpp"

#ifdef __linux__

#include "audio/host/VST3RunLoopEventCaller.hpp"

#include <pluginterfaces/gui/iplugview.h>

#include <QTimer>

#include <sys/epoll.h>

#include <atomic>
#include <map>
#include <memory>
#include <thread>

namespace YADAW::Audio::Plugin
{
class VST3Plugin;
}

namespace YADAW::Audio::Host
{
class VST3RunLoop: public Steinberg::Linux::IRunLoop
{
private:
    struct EventCaller
    {
        Steinberg::Linux::IEventHandler* eventHandler;
        int fd;
        void operator()() const
        {
            eventHandler->onFDIsSet(fd);
        }
        friend bool operator<(EventCaller lhs, EventCaller rhs)
        {
            return std::tie(lhs.eventHandler, lhs.fd) < std::tie(rhs.eventHandler, rhs.fd);
        }
    };
public:
    VST3RunLoop();
    ~VST3RunLoop();
public:
    Steinberg::tresult queryInterface(const Steinberg::TUID iid, void** obj) override;
    Steinberg::uint32 addRef() override;
    Steinberg::uint32 release() override;
public:
    Steinberg::tresult registerEventHandler(
        Steinberg::Linux::IEventHandler* handler,
        Steinberg::Linux::FileDescriptor fd) override;
    Steinberg::tresult unregisterEventHandler(
        Steinberg::Linux::IEventHandler* handler) override;
    Steinberg::tresult registerTimer(
        Steinberg::Linux::ITimerHandler* handler,
        Steinberg::Linux::TimerInterval milliseconds) override;
    Steinberg::tresult unregisterTimer(
        Steinberg::Linux::ITimerHandler* handler) override;
private:
    std::atomic<std::uint32_t> refCount_;
    const YADAW::Audio::Plugin::VST3Plugin* plugin_;
    std::map<
        Steinberg::Linux::IEventHandler*,
        std::set<Steinberg::Linux::FileDescriptor>
    > fileDescriptors_;
    std::map<
        Steinberg::Linux::ITimerHandler*,
        YADAW::Util::AlignedStorage<QTimer>
    > timerHandlers_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_HOST_VST3RUNLOOP