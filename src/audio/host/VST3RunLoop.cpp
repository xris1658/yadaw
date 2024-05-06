#ifdef __linux__

#include "VST3RunLoop.hpp"

using namespace Steinberg::Linux;

namespace YADAW::Audio::Host
{
VST3RunLoop::VST3RunLoop() {}

VST3RunLoop& VST3RunLoop::instance()
{
    static VST3RunLoop ret;
    return ret;
}

Steinberg::tresult VST3RunLoop::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown::iid, IRunLoop)
    QUERY_INTERFACE(_iid, obj, IRunLoop::iid, IRunLoop)
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 VST3RunLoop::addRef()
{
    return 1;
}

Steinberg::uint32 VST3RunLoop::release()
{
    return 1;
}

Steinberg::tresult VST3RunLoop::registerEventHandler(
    Steinberg::Linux::IEventHandler* handler,
    Steinberg::Linux::FileDescriptor fd)
{
    return Steinberg::kNotImplemented;
}

Steinberg::tresult VST3RunLoop::unregisterEventHandler(
    Steinberg::Linux::IEventHandler* handler)
{
    return Steinberg::kNotImplemented;
}

Steinberg::tresult VST3RunLoop::registerTimer(
    Steinberg::Linux::ITimerHandler* handler,
    Steinberg::Linux::TimerInterval milliseconds)
{
    auto it = timerHandlers_.find(handler);
    if(it != timerHandlers_.end())
    {
        return Steinberg::kResultFalse;
    }
    handler->addRef();
    auto timer = std::make_unique<QTimer>();
    timer->setInterval(milliseconds);
    timer->setSingleShot(false);
    timer->setTimerType(Qt::TimerType::PreciseTimer);
    timer->callOnTimeout(
        [handler]()
        {
            handler->onTimer();
        }
    );
    timer->start();
    timerHandlers_.emplace(
        handler,
        std::move(timer)
    );
    return Steinberg::kResultOk;
}

Steinberg::tresult VST3RunLoop::unregisterTimer(
    Steinberg::Linux::ITimerHandler* handler)
{
    auto it = timerHandlers_.find(handler);
    if(it == timerHandlers_.end())
    {
        return Steinberg::kResultFalse;
    }
    timerHandlers_.erase(it);
    return Steinberg::kResultOk;
}
}

#endif