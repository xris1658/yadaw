#ifndef YADAW_SRC_AUDIO_HOST_VST3RUNLOOPEVENTCALLER
#define YADAW_SRC_AUDIO_HOST_VST3RUNLOOPEVENTCALLER

#if __linux__

#include <pluginterfaces/gui/iplugview.h>

#include <QObject>

namespace YADAW::Audio::Host
{
class VST3RunLoopEventCaller: public QObject
{
    Q_OBJECT
public:
    VST3RunLoopEventCaller();
signals:
    void setFD(Steinberg::Linux::IEventHandler* eventHandler,
        Steinberg::Linux::FileDescriptor fd);
private slots:
    void onSetFD(Steinberg::Linux::IEventHandler* eventHandler,
        Steinberg::Linux::FileDescriptor fd);
};
}

#endif

#endif // YADAW_SRC_AUDIO_HOST_VST3RUNLOOPEVENTCALLER