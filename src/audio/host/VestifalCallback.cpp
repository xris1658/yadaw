#include "VestifalCallback.hpp"

#include "audio/plugin/VestifalPlugin.hpp"

#include <cstring>
#include <mutex>

namespace YADAW::Audio::Host
{
thread_local bool uniquePluginIdShouldBeZero;
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
        auto ret = (uniquePluginId == 0 && (!uniquePluginIdShouldBeZero))?
            effect->uniqueId: uniquePluginId;
        return ret;
    }
    case audioMasterGetTime:
    {
        auto plugin = static_cast<YADAW::Audio::Plugin::VestifalPlugin*>(effect->user);
        return reinterpret_cast<std::intptr_t>(plugin->timeInfo());
    }
    case audioMasterGetVendorName:
        std::strcpy(static_cast<char*>(ptr), "xris1658");
        return 0;
    case audioMasterGetProductName:
        std::strcpy(static_cast<char*>(ptr), "YADAW");
        return 0;
    case audioMasterCanDo:
        return std::strcmp(reinterpret_cast<const char*>(ptr), "shellCategory") == 0
            || std::strcmp(reinterpret_cast<const char*>(ptr), "supportShell") == 0;
    }
    return 0;
}

void setUniquePluginIdOnCurrentThread(std::int32_t uniqueId)
{
    uniquePluginId = uniqueId;
}

void setUniquePluginShouldBeZeroOnCurrentThread(bool value)
{
    uniquePluginIdShouldBeZero = value;
}
}
