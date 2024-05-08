#ifndef YADAW_SRC_AUDIO_HOST_VST3RUNLOOP
#define YADAW_SRC_AUDIO_HOST_VST3RUNLOOP

#ifdef __linux__

#include <pluginterfaces/gui/iplugview.h>

#include <QTimer>

#include <sys/epoll.h>

#include <atomic>
#include <map>
#include <memory>
#include <thread>

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
private:
    VST3RunLoop();
    ~VST3RunLoop();
public:
    static VST3RunLoop& instance();
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
public:
    void setMainThreadContext(const QObject& mainThreadContext);
    void stop();
private:
    bool tryEpollCreateIfNeeded();
private:
    int epollFD_;
    std::atomic_flag running_;
    const QObject* mainThreadContext_;
    std::map<
        Steinberg::Linux::IEventHandler*,
        std::map<EventCaller, epoll_event>
    > eventHandlers_;
    std::map<
        Steinberg::Linux::ITimerHandler*,
        std::unique_ptr<QTimer>>
    timerHandlers_;
    std::thread epollThread_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_HOST_VST3RUNLOOP