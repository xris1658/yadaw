#ifndef YADAW_SRC_AUDIO_HOST_EVENTFILEDESCRIPTORSUPPORT
#define YADAW_SRC_AUDIO_HOST_EVENTFILEDESCRIPTORSUPPORT

#if __linux__

#include "audio/plugin/IAudioPlugin.hpp"

#include <pluginterfaces/gui/iplugview.h>

#include <clap/ext/posix-fd-support.h>

#include <QObject>
#include <QTimer>

#include <sys/epoll.h>

#include <atomic>
#include <variant>

namespace YADAW::Audio::Host
{
class EventFileDescriptorSupport: public QObject
{
    Q_OBJECT
public:
    struct Info
    {
        YADAW::Audio::Plugin::IAudioPlugin::Format format;
        struct VST3Info
        {
            Steinberg::Linux::IEventHandler* eventHandler;
        };
        struct CLAPInfo
        {
            const clap_plugin_t* plugin;
            const clap_plugin_posix_fd_support_t* posixFDSupport;
            clap_posix_fd_flags_t flags;
        };
        std::variant<VST3Info, CLAPInfo> data;
    };
private:
    EventFileDescriptorSupport();
public:
    static EventFileDescriptorSupport& instance();
    ~EventFileDescriptorSupport();
public:
    void start();
    void stop();
private:
    void notify(int fd, Info& info);
public:
    bool add(int fd, const Info& info);
    bool remove(int fd);
    void clear();
    bool modifyCLAP(int fd, clap_posix_fd_flags_t flags);
private:
    std::map<int, Info> eventFDs_;
    QMetaObject::Connection connectToTimer_;
    int epollFD_;
};
}

#endif

#endif //YADAW_SRC_AUDIO_HOST_EVENTFILEDESCRIPTORSUPPORT
