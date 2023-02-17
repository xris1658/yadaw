#ifndef YADAW_SRC_AUDIO_ENGINE_DEVICENODE
#define YADAW_SRC_AUDIO_ENGINE_DEVICENODE

#include "audio/device/IDevice.hpp"

namespace YADAW::Audio::Engine
{
class DeviceNode
{
public:
    using ProcessFunc = void(YADAW::Audio::Device::IDevice*,
        const YADAW::Audio::Device::AudioProcessData<float>&);
private:
    static void dummyProcess(YADAW::Audio::Device::IDevice* device,
        const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) {}
public:
    DeviceNode();
    DeviceNode(YADAW::Audio::Device::IDevice* device, ProcessFunc* processFunc);
    DeviceNode(const DeviceNode&) = default;
    DeviceNode(DeviceNode&&) noexcept = default;
    DeviceNode& operator=(const DeviceNode&) = default;
    DeviceNode& operator=(DeviceNode&&) = default;
    ~DeviceNode() noexcept = default;
public:
    YADAW::Audio::Device::IDevice* device();
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
private:
    YADAW::Audio::Device::IDevice* device_;
    ProcessFunc* processFunc_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_DEVICENODE
