#if __APPLE__

#include "native/Thread.hpp"

#include "util/IntegerRange.hpp"

#include <pthread.h>
#include <sched.h>

#include <bitset>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity)
{
    // TODO
    return false;
}

std::thread::native_handle_type getCurrentThreadHandle()
{
    return pthread_self();
}

void setThreadPriorityToTimecritical(std::thread::native_handle_type handle)
{
    sched_param sch;
    if(auto priority = sched_get_priority_max(SCHED_FIFO); priority != -1)
    {
        sch.sched_priority = priority;
        pthread_setschedparam(handle, SCHED_FIFO, &sch);
    }
}
}

#endif