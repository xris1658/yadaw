#if __linux__

#include "VST3RunLoopEventCaller.hpp"

namespace YADAW::Audio::Host
{
VST3RunLoopEventCaller::VST3RunLoopEventCaller():
    QObject(nullptr)
{
    QObject::connect(
        this, &VST3RunLoopEventCaller::setFD,
        this, &VST3RunLoopEventCaller::onSetFD,
        Qt::QueuedConnection
    );
}

void VST3RunLoopEventCaller::onSetFD(Steinberg::Linux::IEventHandler* eventHandler, Steinberg::Linux::FileDescriptor fd)
{
    eventHandler->onFDIsSet(fd);
}
}

#endif