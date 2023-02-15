#include "PluginController.hpp"

namespace YADAW::Controller
{
using namespace YADAW::Audio::Device;
using namespace YADAW::Audio::Engine;
using namespace YADAW::Audio::Plugin;

// using namespace std::placeholders;

PluginNode createNodeFromCLAP(std::shared_ptr<CLAPPlugin> plugin)
{
    // using std::bind
    // return PluginNode(std::static_pointer_cast<void>(plugin),
    //     std::bind(std::mem_fn(&CLAPPlugin::process), plugin.get(), _1/*const AudioProcessData<float>&*/));
    // using Lambda
    return PluginNode(std::static_pointer_cast<void>(plugin),
        [raw = plugin.get()](const AudioProcessData<float>& data) { raw->process(data); });
}

PluginNode createNodeFromVST3(std::shared_ptr<VST3Plugin> plugin)
{
    // using std::bind
    // return PluginNode(std::static_pointer_cast<void>(plugin),
    //     std::bind(std::mem_fn(&VST3Plugin::process), plugin.get(), _1/*const AudioProcessData<float>&*/));
    // using Lambda
    return PluginNode(std::static_pointer_cast<void>(plugin),
        [raw = plugin.get()](const AudioProcessData<float>& data) { raw->process(data); });
}
}
