#include "DeviceNode.hpp"

namespace YADAW::Audio::Engine
{
DeviceNode::DeviceNode():
    device_(nullptr),
    processFunc_(&DeviceNode::dummyProcess)
{
}

DeviceNode::DeviceNode(YADAW::Audio::Device::IDevice* device, DeviceNode::ProcessFunc* processFunc):
    device_(device),
    processFunc_(processFunc)
{
}

YADAW::Audio::Device::IDevice* DeviceNode::device()
{
    return device_;
}

void DeviceNode::process(const Device::AudioProcessData<float>& audioProcessData)
{
    processFunc_(device_, audioProcessData);
}
}