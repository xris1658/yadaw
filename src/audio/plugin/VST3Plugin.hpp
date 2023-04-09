#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN

#include "audio/host/VST3ComponentHandler.hpp"
#include "audio/plugin/IPlugin.hpp"
#include "audio/plugin/VST3ChannelGroup.hpp"
#include "audio/plugin/VST3PluginGUI.hpp"
#include "audio/plugin/VST3PluginParameter.hpp"

#include <pluginterfaces/base/ipluginbase.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstmessage.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
class VST3Plugin: public YADAW::Audio::Plugin::IPlugin
{
public:
    using InitEntry = bool(*)();
    using FactoryEntry = Steinberg::IPluginFactory*(*)();
    using ExitEntry = bool(*)();
public:
    VST3Plugin();
    VST3Plugin(InitEntry initEntry, FactoryEntry factoryEntry, ExitEntry exitEntry);
    bool createPlugin(const Steinberg::TUID& uid);
    ~VST3Plugin();
public:
    Steinberg::IPluginFactory* factory();
    Steinberg::Vst::IComponent* component();
    Steinberg::Vst::IAudioProcessor* audioProcessor();
    Steinberg::Vst::IEditController* editController();
    const Steinberg::Vst::ProcessSetup& processSetup();
public:
    bool initialize(double sampleRate, std::int32_t maxSampleCount) override;
    bool uninitialize() override;
    bool activate() override;
    bool deactivate() override;
    bool startProcessing() override;
    bool stopProcessing() override;
    Format format() override;
    Status status() override;
    IPluginGUI* gui() override;
    IPluginParameter* parameter() override;
public:
    VST3PluginGUI* pluginGUI();
    VST3PluginParameter* pluginParameter();
public:
    int audioInputGroupCount() const override;
    int audioOutputGroupCount() const override;
    const IChannelGroup* audioInputGroupAt(int index) const override;
    const IChannelGroup* audioOutputGroupAt(int index) const override;
    bool isAudioInputGroupActivated(int index) const;
    bool activateAudioInputGroup(int index, bool state);
    bool isAudioOutputGroupActivated(int index) const;
    bool activateAudioOutputGroup(int index, bool state);
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
public:
    YADAW::Audio::Host::VST3ComponentHandler* componentHandler();
private:
    void prepareAudioRelatedInfo();
    void clearAudioRelatedInfo();
    void prepareProcessData(Steinberg::int32 processMode);
    void resetProcessData();
    bool createEditController();
    bool initializeEditController();
    bool uninitializeEditController();
    bool destroyEditController();
private:
    IPlugin::Status status_ = IPlugin::Status::Empty;
    std::int32_t componentAndEditControllerUnified_ = 0;
    ExitEntry exitEntry_ = nullptr;
    Steinberg::IPluginFactory* factory_ = nullptr;
    Steinberg::Vst::IComponent* component_ = nullptr;
    Steinberg::Vst::IAudioProcessor* audioProcessor_ = nullptr;
    Steinberg::Vst::IEditController* editController_ = nullptr;
    Steinberg::Vst::IConnectionPoint* componentPoint_ = nullptr;
    Steinberg::Vst::IConnectionPoint* editControllerPoint_ = nullptr;
    std::vector<YADAW::Audio::Plugin::VST3ChannelGroup> audioInputChannelGroup_;
    std::vector<YADAW::Audio::Plugin::VST3ChannelGroup> audioOutputChannelGroup_;
    std::vector<bool> audioInputBusActivated_;
    std::vector<bool> audioOutputBusActivated_;
    Steinberg::Vst::ProcessSetup processSetup_ = {};
    Steinberg::Vst::ProcessData processData_= {};
    std::vector<Steinberg::Vst::AudioBusBuffers> inputBuffers_;
    std::vector<Steinberg::Vst::AudioBusBuffers> outputBuffers_;
    std::unique_ptr<VST3PluginGUI> gui_;
    std::unique_ptr<VST3PluginParameter> parameter_;
    std::unique_ptr<YADAW::Audio::Host::VST3ComponentHandler> componentHandler_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN
