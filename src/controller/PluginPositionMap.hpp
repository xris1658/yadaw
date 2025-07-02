#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP

#include <cstdint>

namespace YADAW::Controller
{
struct PluginPosition
{
    enum InChannelPosition
    {
        Instrument,
        Insert
    };
    void* model = nullptr;
    InChannelPosition position = InChannelPosition::Insert;
    std::uint32_t index = 0U;
};
}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
