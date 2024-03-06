#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN

#include "audio/host/VST3ComponentHandler.hpp"
#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/plugin/VST3AudioChannelGroup.hpp"
#include "audio/plugin/VST3EventProcessor.hpp"
#include "audio/plugin/VST3PluginGUI.hpp"
#include "audio/plugin/VST3PluginParameter.hpp"
#include "native/VST3Native.hpp"

#include <pluginterfaces/base/ipluginbase.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstmessage.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
class VST3Plugin: public YADAW::Audio::Plugin::IAudioPlugin
{
public:
    using FactoryEntry = Steinberg::IPluginFactory*(*)();
    using ExitEntry = bool(*)();
public:
    VST3Plugin();
    VST3Plugin(YADAW::Native::VST3InitEntry initEntry, FactoryEntry factoryEntry, ExitEntry exitEntry,
        void* libraryHandle = nullptr);
    VST3Plugin(VST3Plugin&& rhs);
    bool createPlugin(const Steinberg::TUID& uid);
    void destroyPlugin();
    ~VST3Plugin();
public:
    Steinberg::IPluginFactory* factory();
    Steinberg::Vst::IComponent* component();
    Steinberg::Vst::IAudioProcessor* audioProcessor();
    Steinberg::Vst::IEditController* editController();
    const Steinberg::Vst::ProcessSetup& processSetup();
public:
    bool setChannelGroups(YADAW::Audio::Base::ChannelGroupType* inputs, std::uint32_t inputCount,
        YADAW::Audio::Base::ChannelGroupType* outputs, std::uint32_t outputCount);
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
    VST3PluginGUI* pluginGUI();
    const VST3PluginGUI* pluginGUI() const;
    VST3PluginParameter* pluginParameter();
    const VST3PluginParameter* pluginParameter() const;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    IAudioDevice::OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    IAudioDevice::OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    bool isAudioInputGroupActivated(std::uint32_t index) const;
    bool activateAudioInputGroup(std::uint32_t index, bool state);
    bool isAudioOutputGroupActivated(std::uint32_t index) const;
    bool activateAudioOutputGroup(std::uint32_t index, bool state);
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
private:
    void doProcess(const Device::AudioProcessData<float>& audioProcessData);
    void blankProcess(const Device::AudioProcessData<float>& audioProcessData);
public:
    Steinberg::Vst::IComponentHandler* componentHandler();
    YADAW::Audio::Plugin::VST3EventProcessor* eventProcessor();
private:
    void prepareAudioRelatedInfo();
    void clearAudioRelatedInfo();
    void prepareProcessData(Steinberg::int32 processMode);
    void resetProcessData();
    bool createEditController();
    bool initializeEditController();
    bool uninitializeEditController();
    bool destroyEditController();
public:
    void refreshParameterInfo();
    void setComponentHandler(Steinberg::Vst::IComponentHandler& componentHandler);
    void setProcessContext(Steinberg::Vst::ProcessContext& processContext);
    void setParameterChanges(Steinberg::Vst::IParameterChanges& inputParameterChanges, Steinberg::Vst::IParameterChanges* outputParameterChanges);
    void setEventList(Steinberg::Vst::IEventList* inputEventList, Steinberg::Vst::IEventList* outputEventList);
private:
    IAudioPlugin::Status status_ = IAudioPlugin::Status::Empty;
    std::int32_t unified_ = 0;
    ExitEntry exitEntry_ = nullptr;
    Steinberg::IPluginFactory* factory_ = nullptr;
    Steinberg::Vst::IComponent* component_ = nullptr;
    Steinberg::Vst::IAudioProcessor* audioProcessor_ = nullptr;
    mutable Steinberg::Vst::IEditController* editController_ = nullptr;
    Steinberg::Vst::IConnectionPoint* componentPoint_ = nullptr;
    Steinberg::Vst::IConnectionPoint* editControllerPoint_ = nullptr;
    std::vector<YADAW::Audio::Plugin::VST3AudioChannelGroup> audioInputChannelGroup_;
    std::vector<YADAW::Audio::Plugin::VST3AudioChannelGroup> audioOutputChannelGroup_;
    std::vector<bool> audioInputBusActivated_;
    std::vector<bool> audioOutputBusActivated_;
    Steinberg::Vst::ProcessSetup processSetup_ = {};
    Steinberg::Vst::ProcessData processData_ = {};
    std::vector<Steinberg::Vst::AudioBusBuffers> inputBuffers_;
    std::vector<Steinberg::Vst::AudioBusBuffers> outputBuffers_;
    mutable std::unique_ptr<VST3PluginGUI> gui_;
    mutable std::unique_ptr<VST3PluginParameter> parameter_;
    Steinberg::Vst::IComponentHandler* componentHandler_ = nullptr;
    std::unique_ptr<YADAW::Audio::Plugin::VST3EventProcessor> eventProcessor_;
    static constexpr decltype(&VST3Plugin::doProcess) processFunc_[2] = {
        &VST3Plugin::blankProcess,
        &VST3Plugin::doProcess
    };
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VST3PLUGIN
