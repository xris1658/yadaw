#include "VestifalCallback.hpp"

#include <mutex>

namespace YADAW::Audio::Host
{
thread_local std::int32_t uniquePluginId;

std::intptr_t vestifalHostCallback(AEffect* effect, std::int32_t opcode, std::int32_t input, std::intptr_t opt, void* ptr,
    float value)
{
    switch(opcode)
    {
    case audioMasterVersion:
        return 2400;
    case audioMasterGetCurrentUniqueId:
    {
        auto ret = uniquePluginId;
        return ret;
    }
    case audioMasterGetVendorName:
        std::strcpy(static_cast<char*>(ptr), "xris1658");
        return 0;
    case audioMasterGetProductName:
        std::strcpy(static_cast<char*>(ptr), "YADAW");
        return 0;
    }
    return 0;
}

void setUniquePluginIdOnCurrentThread(std::int32_t uniqueId)
{
    uniquePluginId = uniqueId;
}
}