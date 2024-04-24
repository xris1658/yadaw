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
struct PluginContext
{
    enum Position
    {
        Instrument,
        Insert
    };
    void* model = nullptr;
    Position position = Position::Insert;
    std::uint32_t index = 0U;
};

using PluginContextMap = std::map<
    YADAW::Audio::Plugin::IAudioPlugin*,
    PluginContext
>;

PluginContextMap& appPluginContextMap();

}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
