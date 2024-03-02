#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP

namespace YADAW::Audio::Plugin
{
class IAudioPlugin;
}

namespace YADAW::Model
{
class MixerChannelInsertListModel;
}

namespace YADAW::Controller
{
struct PluginContext
{
    YADAW::Model::MixerChannelInsertListModel* insertListModel = nullptr;
    std::uint32_t insertIndex = 0U;
};

using PluginContextMap = std::map<
    YADAW::Audio::Plugin::IAudioPlugin*,
    PluginContext
>;

PluginContextMap& appPluginContextMap();

}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXTMAP
