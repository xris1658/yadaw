#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN

#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/CLAPAudioChannelGroup.hpp"
#include "audio/plugin/CLAPEventProcessor.hpp"
#include "audio/plugin/CLAPPluginGUI.hpp"
#include "audio/plugin/CLAPPluginParameter.hpp"
#include "audio/plugin/IAudioPlugin.hpp"

#include <clap/entry.h>
#include <clap/plugin.h>
#include <clap/audio-buffer.h>
#include <clap/ext/audio-ports.h>
#include <clap/ext/audio-ports-config.h>
#include <clap/ext/note-ports.h>
#include <clap/ext/gui.h>
#include <clap/ext/latency.h>
#include <clap/ext/params.h>
#include <clap/ext/tail.h>
#include <clap/factory/plugin-factory.h>

#include <QString>

#include <vector>

namespace YADAW::Audio::Plugin
{
class CLAPPlugin: public YADAW::Audio::Plugin::IAudioPlugin
{
    friend class YADAW::Audio::Host::CLAPHost;
public:
    class AudioPortsConfig
    {
    public:
        AudioPortsConfig();
        AudioPortsConfig(
            const clap_plugin_audio_ports_config& config, const clap_plugin& plugin,
            std::uint32_t index);
        AudioPortsConfig(const AudioPortsConfig& rhs) = default;
        AudioPortsConfig& operator=(const AudioPortsConfig& rhs) = default;
        ~AudioPortsConfig() = default;
    public:
        QUtf8StringView name() const;
        clap_id id() const;
        std::uint32_t inputCount() const;
        std::uint32_t outputCount() const;
        bool hasMainInput() const;
        bool hasMainOutput() const;
        std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t> mainInputType() const;
        std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t> mainOutputType() const;
    private:
        clap_audio_ports_config_t audioPortsConfig_;
    };
public:
    CLAPPlugin();
    CLAPPlugin(const clap_plugin_entry* entry, const QString& path);
    bool createPlugin(const char* id);
    ~CLAPPlugin();
public:
    const clap_plugin_entry* entry();
    const clap_plugin_factory* factory();
    QString name() const;
public:
    bool initialize(double sampleRate, std::int32_t maxSampleCount) override;
    bool uninitialize() override;
    bool activate() override;
    bool deactivate() override;
    bool startProcessing() override;
    bool stopProcessing() override;
    PluginFormat format() override;
    Status status() override;
    std::uint32_t tailSizeInSamples() override;
    const IPluginGUI* gui() const override;
    IPluginGUI* gui() override;
    const IAudioDeviceParameter* parameter() const override;
    IAudioDeviceParameter* parameter() override;
public:
    const CLAPPluginGUI* pluginGUI() const;
    CLAPPluginGUI* pluginGUI();
    const CLAPPluginParameter* pluginParameter() const;
    CLAPPluginParameter* pluginParameter();
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    IAudioDevice::OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    IAudioDevice::OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
private:
    void doProcess(const Device::AudioProcessData<float>& audioProcessData);
    void blankProcess(const Device::AudioProcessData<float>& audioProcessData);
    void prepareAudioRelatedInfo();
    void clearAudioRelatedInfo();
    void prepareProcessData();
    void resetProcessData();
    void refreshAllParameter();
    void refreshParameterInfo();
public:
    CLAPEventProcessor* eventProcessor();
    clap_process& processData();
    YADAW::Audio::Host::CLAPHost& host();
    void calledOnMainThread();
public:
    std::uint32_t audioPortsConfigCount() const;
    OptionalRef<const AudioPortsConfig> audioPortsConfigAt(std::uint32_t index) const;
    bool selectAudioPortsConfig(clap_id id);
private:
    YADAW::Audio::Host::CLAPHost host_ {*this};
    IAudioPlugin::Status status_ = IAudioPlugin::Status::Empty;
    clap_process_status processStatus_;
    const clap_plugin_entry* entry_ = nullptr;
    const clap_plugin_factory* factory_ = nullptr;
    const clap_plugin* plugin_ = nullptr;
    const clap_plugin_audio_ports* audioPorts_ = nullptr;
    const clap_plugin_audio_ports_config* audioPortsConfig_ = nullptr;
    const clap_plugin_note_ports* notePorts_ = nullptr;
    const clap_plugin_latency* latency_ = nullptr;
    const clap_plugin_tail* tail_ = nullptr;
    double sampleRate_ = 0;
    std::int32_t minBlockSize_ = 1;
    std::int32_t maxBlockSize_ = INT32_MAX;
    std::vector<CLAPAudioChannelGroup> inputChannelGroups_;
    std::vector<CLAPAudioChannelGroup> outputChannelGroups_;
    std::vector<clap_audio_buffer> inputBuffers_;
    std::vector<clap_audio_buffer> outputBuffers_;
    clap_process processData_
    {
        -1,
        0,
        nullptr,
        nullptr,
        nullptr,
        0,
        0,
        nullptr,
        nullptr
    };
    std::unique_ptr<CLAPEventProcessor> eventProcessor_;
    mutable std::unique_ptr<CLAPPluginGUI> gui_;
    mutable std::unique_ptr<CLAPPluginParameter> parameter_;
    static constexpr decltype(&CLAPPlugin::doProcess) processFunc_[2] = {
        &CLAPPlugin::blankProcess,
        &CLAPPlugin::doProcess
    };
    std::vector<AudioPortsConfig> audioPortsConfigs_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN
