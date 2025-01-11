#if __linux__

#include "native/Thread.hpp"

#include "util/IntegerRange.hpp"

#include <pthread.h>
#include <sched.h>

#include <bitset>

namespace YADAW::Native
{
bool setThreadAffinity(std::thread::native_handle_type handle, std::uint64_t affinity)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    std::bitset<64> bits(affinity);
    FOR_RANGE0(i, 64)
    {
        if(bits.test(i))
        {
            CPU_SET(i, &cpuset);
        }
    }
    return pthread_setaffinity_np(handle, sizeof(cpuset), &cpuset) == 0;
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