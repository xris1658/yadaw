#ifndef YADAW_SRC_AUDIO_ENGINE_PLUGINNODE
#define YADAW_SRC_AUDIO_ENGINE_PLUGINNODE

#include "audio/device/IDevice.hpp"

#include <functional>

namespace YADAW::Audio::Engine
{
using PluginProcessFuncType = void(const YADAW::Audio::Device::AudioProcessData<float>&);

class PluginNode
{
public:
    // ProcessType will be used as DeviceProcessFuncType
    template<typename ProcessType>
    PluginNode(std::shared_ptr<void> device, ProcessType&& process):
        device_(std::move(device)),
        process_(std::forward<ProcessType>(process))
    {
    }
    PluginNode(const PluginNode&) = delete;
    PluginNode(PluginNode&&) = default;
    PluginNode& operator=(const PluginNode&) = delete;
    PluginNode& operator=(PluginNode&&) = default;
    ~PluginNode() = default;
public:
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
private:
    std::shared_ptr<void> device_;
    std::function<PluginProcessFuncType> process_;
};

}

#endif //YADAW_SRC_AUDIO_ENGINE_PLUGINNODE
