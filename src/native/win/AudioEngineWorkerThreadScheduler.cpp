#if _WIN32

#include "native/AudioEngineWorkerThreadScheduler.hpp"

#include <avrt.h>

namespace YADAW::Audio::Engine
{
AudioEngineWorkerThreadPool::ThreadScheduler::ThreadScheduler(
    const AudioEngineWorkerThreadPool& sender, const InitArgs& initArgs)
{
    // https://learn.microsoft.com/windows/win32/procthread/multimedia-class-scheduler-service#registry-settings
    mmcssTask_ = AvSetMmThreadCharacteristicsW(L"Pro Audio", &mmcssTaskIndex_);
}

AudioEngineWorkerThreadPool::ThreadScheduler::~ThreadScheduler()
{
    if(mmcssTask_)
    {
        AvRevertMmThreadCharacteristics(mmcssTask_);
    }
}
}

#endif