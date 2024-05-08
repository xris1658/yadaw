#ifdef __linux__

#include "VST3RunLoop.hpp"

#include "util/IntegerRange.hpp"

#include <array>

using namespace Steinberg::Linux;

namespace YADAW::Audio::Host
{
VST3RunLoop::VST3RunLoop():
    epollFD_(-1),
    running_(ATOMIC_FLAG_INIT)
{
    tryEpollCreateIfNeeded();
}

VST3RunLoop::~VST3RunLoop()
{
    running_.clear(std::memory_order_release);
    if(epollThread_.joinable())
    {
        epollThread_.join();
    }
}

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
    if(handler && fd >= 0)
    {
        return Steinberg::kInvalidArgument;
    }
    tryEpollCreateIfNeeded();
    if(epollFD_ < 0)
    {
        return Steinberg::kResultFalse;
    }
    auto& eventCallers = eventHandlers_[handler];
    auto [it, inserted] = eventCallers.emplace(
        EventCaller {
            .eventHandler = handler, .fd = fd
        },
        epoll_event {}
    );
    if(!inserted)
    {
        return Steinberg::kInvalidArgument;
    }
    else
    {
        auto& [eventCaller, epollEvent] = *it;
        epollEvent.events = EPOLLIN;
        epollEvent.data.ptr = static_cast<void*>(
            const_cast<EventCaller*>(&eventCaller)
        );
        auto controlResult = epoll_ctl(
            epollFD_, EPOLL_CTL_ADD, fd, &epollEvent
        );
        if(controlResult == 0)
        {
            return Steinberg::kResultOk;
        }
        else
        {
            return Steinberg::kResultFalse;
        }
    }
}

Steinberg::tresult VST3RunLoop::unregisterEventHandler(
    Steinberg::Linux::IEventHandler* handler)
{
    auto it = eventHandlers_.find(handler);
    if(it != eventHandlers_.end())
    {
        auto& map = it->second;
        for(auto& [eventCaller, epollEvent]: map)
        {
            epoll_ctl(
                epollFD_, EPOLL_CTL_DEL,
                eventCaller.fd, &epollEvent
            );
        }
        return Steinberg::kResultOk;
    }
    else
    {
        return Steinberg::kInvalidArgument;
    }
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
        mainThreadContext_,
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

bool VST3RunLoop::tryEpollCreateIfNeeded()
{
    if(epollFD_ < 0)
    {
        epollFD_ = epoll_create1(0);
        if(epollFD_ >= 0)
        {
            running_.test_and_set(std::memory_order_release);
            epollThread_ = std::thread(
                [this]()
                {
                    std::array<epoll_event, 128> epollEvents;
                    while(running_.test(std::memory_order_acquire))
                    {
                        // FIXME: Might sleep indefinitely here
                        auto waitResult = epoll_wait(
                            epollFD_,
                            epollEvents.data(),
                            epollEvents.size(),
                            -1
                        );
                        if(waitResult >= 0)
                        {
                            FOR_RANGE0(i, waitResult)
                            {
                                auto eventCaller = static_cast<EventCaller*>(
                                    epollEvents[i].data.ptr
                                );
                                (*eventCaller)();
                            }
                        }
                    }
                }
            );
        }
    }
    return epollFD_ >= 0;
}

void VST3RunLoop::setMainThreadContext(const QObject& mainThreadContext)
{
    mainThreadContext_ = &mainThreadContext;
}

void VST3RunLoop::stop()
{
    running_.clear(std::memory_order_release);
}
}

#endif