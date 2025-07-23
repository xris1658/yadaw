#if __linux__

#include "native/AudioEngineWorkerThreadScheduler.hpp"

#include "native/Thread.hpp"

#include <sched.h>

namespace YADAW::Audio::Engine
{
AudioEngineWorkerThreadPool::ThreadScheduler::ThreadScheduler(
    const AudioEngineWorkerThreadPool& sender, const InitArgs& initArgs)
{
    auto handle = YADAW::Native::getCurrentThreadHandle();
    int policy;
    sched_param schedParam;
    pthread_getschedparam(handle, &policy, &schedParam);
    schedParam.sched_priority = 20;
    pthread_setschedparam(handle, SCHED_FIFO, &schedParam); // TODO: Use `SCHED_DEADLINE`?
}

AudioEngineWorkerThreadPool::ThreadScheduler::~ThreadScheduler()
{}
}

#endif