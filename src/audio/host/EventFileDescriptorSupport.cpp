#if __linux__

#include "EventFileDescriptorSupport.hpp"

#include "ui/Runtime.hpp"
#include "util/IntegerRange.hpp"

#include <QCoreApplication>

#include <sys/epoll.h>

#include <array>

namespace YADAW::Audio::Host
{
std::array<epoll_event, 64> epollEvents;

std::uint32_t eventFlagsFromCLAP(clap_posix_fd_flags_t flags)
{
    std::uint32_t ret = 0U;
    if(flags & CLAP_POSIX_FD_READ)
    {
        flags |= EPOLL_EVENTS::EPOLLIN;
    }
    if(flags & CLAP_POSIX_FD_WRITE)
    {
        flags |= EPOLL_EVENTS::EPOLLOUT;
    }
    if(flags & CLAP_POSIX_FD_ERROR)
    {
        flags |= EPOLL_EVENTS::EPOLLERR;
    }
    return ret;
}

EventFileDescriptorSupport::EventFileDescriptorSupport():
    epollFD_(epoll_create1(0))
{}

EventFileDescriptorSupport& EventFileDescriptorSupport::instance()
{
    static EventFileDescriptorSupport ret;
    return ret;
}

EventFileDescriptorSupport::~EventFileDescriptorSupport()
{
    stop();
    clear();
    close(epollFD_);
}

void EventFileDescriptorSupport::start(QTimer& timer)
{
    connectToTimer_ = timer.callOnTimeout(
        QCoreApplication::instance(),
        [this]()
        {
            auto epollEventCount = epoll_wait(
                epollFD_, epollEvents.data(), epollEvents.size(), 0
            );
            if(epollEventCount > 0)
            {
                FOR_RANGE0(i, epollEventCount)
                {
                    auto pairPtr = static_cast<decltype(eventFDs_)::pointer>(
                        epollEvents[i].data.ptr
                    );
                    auto& [fd, info] = *pairPtr;
                    notify(fd, info);
                }
            }
        }
    );
}

void EventFileDescriptorSupport::stop()
{
    if(connectToTimer_)
    {
        QObject::disconnect(connectToTimer_);
        connectToTimer_ = {};
    }
}

void EventFileDescriptorSupport::notify(int fd, Info& info)
{
    auto& [format, data] = info;
    switch(format)
    {
    case YADAW::Audio::Plugin::PluginFormat::VST3:
    {
        auto eventHandler = std::get<Info::VST3Info>(data).eventHandler;
        // Call the following on the main thread
        eventHandler->onFDIsSet(fd);
        break;
    }
    case YADAW::Audio::Plugin::PluginFormat::CLAP:
    {
        auto& clapInfo = std::get<Info::CLAPInfo>(data);
        // Call the following on the main thread
        clapInfo.posixFDSupport->on_fd(
            clapInfo.plugin,
            fd,
            clapInfo.flags
        );
        break;
    }
    default:
    {
        break;
    }
    }
}

bool EventFileDescriptorSupport::add(int fd, const Info& info)
{
    auto ret = false;
    switch(info.format)
    {
    case YADAW::Audio::Plugin::PluginFormat::VST3:
    {
        if(eventFDs_.find(fd) == eventFDs_.end())
        {
            auto it = eventFDs_.emplace(fd, info).first;
            auto& vst3Info = std::get<Info::VST3Info>(it->second.data);
            epoll_event epollEvent {
                EPOLL_EVENTS::EPOLLIN,
                epoll_data_t{&*it},
            };
            epoll_ctl(epollFD_, EPOLL_CTL_ADD, fd, &epollEvent);
            ret = true;
        }
        break;
    }
    case YADAW::Audio::Plugin::PluginFormat::CLAP:
    {
        if(eventFDs_.find(fd) == eventFDs_.end())
        {
            auto it = eventFDs_.emplace(fd, info).first;
            auto& clapInfo = std::get<Info::CLAPInfo>(it->second.data);
            epoll_event epollEvent {
                eventFlagsFromCLAP(clapInfo.flags),
                epoll_data_t{&*it},
            };
            epoll_ctl(epollFD_, EPOLL_CTL_ADD, fd, &epollEvent);
            ret = true;
        }
        break;
    }
    default:
    {
        ret = false;
        break;
    }
    }
    return ret;
}

bool EventFileDescriptorSupport::remove(int fd)
{
    auto it = eventFDs_.find(fd);
    if(it != eventFDs_.end())
    {
        epoll_ctl(epollFD_, EPOLL_CTL_DEL, fd, nullptr);
        eventFDs_.erase(it);
        return true;
    }
    return false;
}

void EventFileDescriptorSupport::clear()
{
    //
}

bool EventFileDescriptorSupport::modifyCLAP(int fd, clap_posix_fd_flags_t flags)
{
    auto newFlag = eventFlagsFromCLAP(flags);
    auto it = eventFDs_.find(fd);
    if(it != eventFDs_.end())
    {
        auto& info = it->second.data;
        if(it->second.format == YADAW::Audio::Plugin::PluginFormat::CLAP)
        {
            auto& clapInfo = std::get<Info::CLAPInfo>(info);
            clapInfo.flags = flags;
            epoll_event epollEvent {
                eventFlagsFromCLAP(clapInfo.flags),
                epoll_data_t{&*it},
            };
            epoll_ctl(epollFD_, EPOLL_CTL_MOD, fd, &epollEvent);
        }
        return true;
    }
    return false;
}
}

#endif