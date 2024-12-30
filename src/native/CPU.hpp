#ifndef YADAW_SRC_NATIVE_CPU
#define YADAW_SRC_NATIVE_CPU

#include "util/VectorTypes.hpp"

#include <cstdint>
#include <map>
#include <set>

#if _WIN32
#include <windows.h>
#endif

namespace YADAW::Native
{
using CPUTopology = std::map<
    std::uint8_t, std::map< // NUMA nodes
        std::uint8_t, std::map< // CPU package
            std::uint8_t, std::set< // physical core
                std::uint16_t // logical processor
            >
        >
    >
>;

// Retrieves topology of CPUs on the machine.
// We'd like to do audio callback tasks in a multi-threaded manner, which
// depends on the CPU topology of the machine.
// On a processor which supports SMT (Simultaneous multithreading), there are
// multiple logical processors on a physical core. The logical processor
// indices differ across operating systems. For example, on Windows, multiple
// logical processors on one physical core have adjacent indices:
// +-----------------------------------------------------------------+
// |                           CPU package                           |
// |                                                                 |
// +   +--------------------------+   +--------------------------+   |
// |   |           Core           |   |           Core           |   |
// |   |   +-------+  +-------+   |   |   +-------+  +-------+   |   |
// |   |   | LP #1 |  | LP #2 |   |   |   | LP #3 |  | LP #4 |   |   |
// |   |   +-------+  +-------+   |   |   +-------+  +-------+   |   |
// |   +--------------------------+   +--------------------------+   |
// |                                                                 |
// +-----------------------------------------------------------------+
// On Linux, the "first" logical processor of every physical cores have
// adjacent indices:
// +-----------------------------------------------------------------+
// |                           CPU package                           |
// |                                                                 |
// +   +--------------------------+   +--------------------------+   |
// |   |           Core           |   |           Core           |   |
// |   |   +-------+  +-------+   |   |   +-------+  +-------+   |   |
// |   |   | LP #1 |  | LP #3 |   |   |   | LP #2 |  | LP #4 |   |   |
// |   |   +-------+  +-------+   |   |   +-------+  +-------+   |   |
// |   +--------------------------+   +--------------------------+   |
// |                                                                 |
// +-----------------------------------------------------------------+
// To ensure consistency to schedule tasks across OSes, we need to get the
// topology before assigning multiple worker threads to certain processors.
//
// On a machine that has multiple processor groups and runs Windows, this
// function depends on getting the correct Windows build version:
// https://learn.microsoft.com/windows/win32/api/winnt/ns-winnt-numa_node_relationship#members
CPUTopology getCPUTopology();

std::uint64_t getProcessAffinity();

extern std::uint32_t cpuidInfo[4];

void fillCPUIDInfo();
}

#endif // YADAW_SRC_NATIVE_CPU