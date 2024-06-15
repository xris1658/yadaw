#ifdef __linux__

#include "VST3RunLoop.hpp"

#include "audio/host/EventFileDescriptorSupport.hpp"
#include "util/IntegerRange.hpp"

#include <QCoreApplication>

#include <array>

using namespace Steinberg::Linux;

namespace YADAW::Audio::Host
{
VST3RunLoop::VST3RunLoop():
    refCount_(1U)
{}

VST3RunLoop::~VST3RunLoop()
{
    auto& eventFDSupport = EventFileDescriptorSupport::instance();
    for(auto& [handler, fdSet]: fileDescriptors_)
    {
        for(auto fd: fdSet)
        {
            eventFDSupport.remove(fd);
        }
        handler->release();
    }
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
    return 1U;
}

Steinberg::uint32 VST3RunLoop::release()
{
    return 1U;
}

Steinberg::tresult VST3RunLoop::registerEventHandler(
    Steinberg::Linux::IEventHandler* handler,
    Steinberg::Linux::FileDescriptor fd)
{
    if((!handler) || fd < 0)
    {
        return Steinberg::kInvalidArgument;
    }
    auto& fdSet = fileDescriptors_[handler];
    if(auto it = fdSet.find(fd); it == fdSet.end())
    {
        auto& eventFDSupport = EventFileDescriptorSupport::instance();
        auto addFDResult = eventFDSupport.add(
            fd,
            EventFileDescriptorSupport::Info {
                .format = YADAW::Audio::Plugin::IAudioPlugin::Format::VST3,
                .data = EventFileDescriptorSupport::Info::VST3Info {
                    .eventHandler = handler
                }
            }
        );
        if(addFDResult)
        {
            fdSet.emplace(fd);
            handler->addRef();
            return Steinberg::kResultOk;
        }
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult VST3RunLoop::unregisterEventHandler(
    Steinberg::Linux::IEventHandler* handler)
{
    auto it = fileDescriptors_.find(handler);
    if(it != fileDescriptors_.end())
    {
        auto& eventFDSupport = EventFileDescriptorSupport::instance();
        auto& fdSet = it->second;
        for(auto fd: fdSet)
        {
            eventFDSupport.remove(fd);
        }
        fileDescriptors_.erase(handler);
        handler->release();
        return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
}

Steinberg::tresult VST3RunLoop::registerTimer(
    Steinberg::Linux::ITimerHandler* handler,
    Steinberg::Linux::TimerInterval milliseconds)
{
    auto it = timerHandlers_.find(handler);
    if(it == timerHandlers_.end())
    {
        it = timerHandlers_.emplace(
            handler,
            YADAW::Util::AlignedStorage<QTimer>()
        ).first;
        auto* timer = new(&(it->second)) QTimer();
        timer->setSingleShot(false);
        timer->setTimerType(Qt::TimerType::PreciseTimer);
        QObject::connect(timer, &QTimer::timeout,
            QCoreApplication::instance(),
            [handler]()
            {
                handler->onTimer();
            }
        );
        timer->start(milliseconds);
        handler->addRef();
        return Steinberg::kResultOk;
    }
    else
    {
        auto* timer = YADAW::Util::AlignHelper<QTimer>::fromAligned(&(it->second));
        timer->start(milliseconds);
        return Steinberg::kResultOk;
    }
}

Steinberg::tresult VST3RunLoop::unregisterTimer(
    Steinberg::Linux::ITimerHandler* handler)
{
    auto it = timerHandlers_.find(handler);
    if(it != timerHandlers_.end())
    {
        auto* timer = YADAW::Util::AlignHelper<QTimer>::fromAligned(&(it->second));
        timer->~QTimer();
        timerHandlers_.erase(it);
        handler->release();
        return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
}
}

#endif