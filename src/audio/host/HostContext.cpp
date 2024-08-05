#include "HostContext.hpp"

namespace YADAW::Audio::Host
{
HostContext& HostContext::instance()
{
    static HostContext ret;
    return ret;
}
}
