#ifndef YADAW_SRC_AUDIO_PLUGIN_IPLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_IPLUGIN

#include "audio/device/IDevice.hpp"
#include "audio/plugin/IPluginGUI.hpp"
#include "audio/plugin/IPluginParameter.hpp"

namespace YADAW::Audio::Plugin
{
class IPlugin: public YADAW::Audio::Device::IDevice
{
public:
    enum Status
    {
        Empty,
        Loaded,
        Created,
        Initialized,
        Activated,
        Processing
    };
    enum class Format
    {
        VST3,
        CLAP
    };
public:
    virtual bool initialize(double sampleRate, std::int32_t maxSampleCount) = 0;
    virtual bool uninitialize() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool startProcessing() = 0;
    virtual bool stopProcessing() = 0;
    virtual Format format() = 0;
    virtual Status status() = 0;
public:
    // Owned by `IPlugin`
    virtual IPluginGUI* gui() = 0;
    // Owned by `IPlugin`
    virtual IPluginParameter* parameter() = 0;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_IPLUGIN
