#ifndef YADAW_SRC_AUDIO_HOST_VST3RUNLOOP
#define YADAW_SRC_AUDIO_HOST_VST3RUNLOOP

#ifdef __linux__

#include <pluginterfaces/gui/iplugview.h>

#include <QTimer>

#include <map>
#include <memory>

namespace YADAW::Audio::Host
{
class VST3RunLoop: public Steinberg::Linux::IRunLoop
{
private:
    VST3RunLoop();
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
private:
    std::map<
        Steinberg::Linux::FileDescriptor,
        Steinberg::Linux::IEventHandler*>
    eventHandlers_;
    std::map<
        Steinberg::Linux::ITimerHandler*,
        std::unique_ptr<QTimer>>
    timerHandlers_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_HOST_VST3RUNLOOP