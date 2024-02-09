#ifndef YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGIN

#include "audio/device/IAudioChannelGroup.hpp"
#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/plugin/VestifalPluginGUI.hpp"
#include "audio/plugin/vestifal/Vestifal.h"

#include <memory>

namespace YADAW::Audio::Plugin
{
class VestifalPlugin: public YADAW::Audio::Plugin::IAudioPlugin
{
public:
public:
    VestifalPlugin();
    VestifalPlugin(VestifalEntry entry, std::int32_t uniqueId = 0);
    VestifalPlugin(const VestifalPlugin&) = delete;
    VestifalPlugin(VestifalPlugin&&) noexcept = default;
    VestifalPlugin& operator=(const VestifalPlugin&) = delete;
    VestifalPlugin& operator=(VestifalPlugin&&) noexcept = default;
    ~VestifalPlugin();
public:
    AEffect* effect();
public:
    bool initialize(double sampleRate, std::int32_t maxSampleCount) override;
    bool uninitialize() override;
    bool activate() override;
    bool deactivate() override;
    bool startProcessing() override;
    bool stopProcessing() override;
    Format format() override;
    Status status() override;
    std::uint32_t tailSizeInSamples() override;
    IPluginGUI* gui() override;
    const IPluginGUI* gui() const override;
    IPluginParameter* parameter() override;
    const IPluginParameter* parameter() const override;
public:
    VestifalPluginGUI* pluginGUI();
    const VestifalPluginGUI* pluginGUI() const;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    IAudioDevice::OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    IAudioDevice::OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
    std::uint32_t audioChannelMapCount() const override;
    OptionalChannelMap audioChannelMapAt(std::uint32_t index) const override;
public:
    TimeInfo* timeInfo() const;
    void setTimeInfo(TimeInfo& timeInfo);
private:
    AEffect* effect_ = nullptr;
    std::int32_t uniqueId = 0;
    Status status_ = Status::Empty;
    mutable std::unique_ptr<VestifalPluginGUI> gui_;
    std::vector<float*> inputs_;
    std::vector<float*> outputs_;
    TimeInfo* timeInfo_ = nullptr;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGIN
