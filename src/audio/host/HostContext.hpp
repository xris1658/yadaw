#ifndef YADAW_SRC_AUDIO_HOST_HOSTCONTEXT
#define YADAW_SRC_AUDIO_HOST_HOSTCONTEXT

#include "concurrent/DoubleBufferSwitch.hpp"

#include <cstdint>

namespace YADAW::Audio::Host
{
struct HostContext
{
    YADAW::Concurrent::DoubleBufferSwitch<std::uint_fast8_t> doubleBufferSwitch;
private:
    HostContext() = default;
public:
    static HostContext& instance();
    HostContext(const HostContext&) = delete;
    ~HostContext() = default;
};
}

#endif // YADAW_SRC_AUDIO_HOST_HOSTCONTEXT
