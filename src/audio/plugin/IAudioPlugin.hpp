#ifndef YADAW_SRC_AUDIO_PLUGIN_IAUDIOPLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_IAUDIOPLUGIN

#include "audio/device/IAudioDevice.hpp"
#include "audio/plugin/AudioPluginBase.hpp"
#include "audio/plugin/IPluginGUI.hpp"
#include "native/PluginFormatSupport.hpp"

#include <limits>

namespace YADAW::Audio::Plugin
{
class IAudioPlugin: public YADAW::Audio::Device::IAudioDevice
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
public:
    static constexpr std::uint32_t InfiniteTailSize = std::numeric_limits<std::uint32_t>::max();
public:
    virtual bool initialize(double sampleRate, std::int32_t maxSampleCount) = 0;
    virtual bool uninitialize() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool startProcessing() = 0;
    virtual bool stopProcessing() = 0;
    virtual PluginFormat format() = 0;
    virtual Status status() = 0;
    virtual std::uint32_t tailSizeInSamples() = 0;
public:
    // Owned by `IPlugin`
    virtual IPluginGUI* gui() = 0;
    virtual const IPluginGUI* gui() const = 0;
};

template<PluginFormat F>
constexpr bool isFormatSupported = YADAW::Native::isPluginFormatSupported<F>;
}

#endif // YADAW_SRC_AUDIO_PLUGIN_IAUDIOPLUGIN
