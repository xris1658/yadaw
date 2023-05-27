#include "ALSABackendController.hpp"

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
YADAW::Audio::Backend::ALSABackend& appALSABackend()
{
    static YADAW::Audio::Backend::ALSABackend ret;
    return ret;
}

YADAW::Model::ALSAInputDeviceListModel& appALSAInputDeviceListModel()
{
    static YADAW::Model::ALSAInputDeviceListModel ret(appALSABackend());
    return ret;
}

YADAW::Model::ALSAOutputDeviceListModel& appALSAOutputDeviceListModel()
{
    static YADAW::Model::ALSAOutputDeviceListModel ret(appALSABackend());
    return ret;
}

bool initializeALSAFromConfig(const YAML::Node& node)
{
    auto& backend = appALSABackend();
    const auto& sampleRateNode = node["sample-rate"];
    auto sampleRate = sampleRateNode.IsDefined()? DefaultSampleRate: sampleRateNode.as<std::uint32_t>();
    const auto& frameSizeNode = node["buffer-size"];
    auto frameSize = frameSizeNode.IsDefined()? DefaultFrameSize: frameSizeNode.as<std::uint32_t>();
    backend.initialize(sampleRate, frameSize);
    const auto& inputDevicesNode = node["inputs"];
    if(!inputDevicesNode.IsDefined())
    {
        // TODO: activate audio devices
    }
}
}