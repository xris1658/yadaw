#ifndef YADAW_SRC_NATIVE_AUDIOENGINEWORKERTHREADSCHEDULER
#define YADAW_SRC_NATIVE_AUDIOENGINEWORKERTHREADSCHEDULER

#include "audio/engine/AudioEngineWorkerThreadPool.hpp"

namespace YADAW::Audio::Engine
{
class AudioEngineWorkerThreadPool::ThreadScheduler
{
public:
    ThreadScheduler(const AudioEngineWorkerThreadPool& sender, const InitArgs& initArgs);
    ~ThreadScheduler();
private:
#if _WIN32
    HANDLE mmcssTask_ = nullptr;
    DWORD mmcssTaskIndex_ = 0;
#endif
};
}

#endif // YADAW_SRC_NATIVE_AUDIOENGINEWORKERTHREADSCHEDULER