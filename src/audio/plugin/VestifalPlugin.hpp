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
    IPluginParameter* parameter() override;
public:
    int audioInputGroupCount() const override;
    int audioOutputGroupCount() const override;
    const Device::IAudioChannelGroup* audioInputGroupAt(int index) const override;
    const Device::IAudioChannelGroup* audioOutputGroupAt(int index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
private:
    AEffect* effect_ = nullptr;
    std::int32_t uniqueId = 0;
    Status status_ = Status::Empty;
    std::unique_ptr<VestifalPluginGUI> gui_;
    std::vector<float*> inputs_;
    std::vector<float*> outputs_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGIN
