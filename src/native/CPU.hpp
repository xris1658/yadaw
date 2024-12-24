#ifndef YADAW_SRC_NATIVE_CPU
#define YADAW_SRC_NATIVE_CPU

#include "util/VectorTypes.hpp"

#include <cstdint>

namespace YADAW::Native
{
using CPUHierarchy = Vec<Vec<Vec<std::uint8_t>>>;

CPUHierarchy getCPUHierarchy();
}

#endif // YADAW_SRC_NATIVE_CPU