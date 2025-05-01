#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP

#include <cstdint>
#include <map>

namespace YADAW::Audio::Plugin
{
class IAudioPlugin;
}

namespace YADAW::Model
{
class MixerChannelListModel;
class MixerChannelInsertListModel;
}

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

using PluginPositionMap = std::map<
    YADAW::Audio::Plugin::IAudioPlugin*,
    PluginPosition
>;

PluginPositionMap& appPluginPosition();

}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
