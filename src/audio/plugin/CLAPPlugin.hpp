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
    Format format() override;
    Status status() override;
    std::uint32_t tailSizeInSamples() override;
    IPluginGUI* gui() override;
    IPluginParameter* parameter() override;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    const IAudioChannelGroup* audioInputGroupAt(std::uint32_t index) const override;
    const IAudioChannelGroup* audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
private:
    void prepareAudioRelatedInfo();
    void clearAudioRelatedInfo();
    void prepareProcessData();
    void resetProcessData();
public:
    CLAPEventProcessor* eventProcessor();
    clap_process& processData();
    YADAW::Audio::Host::CLAPHost& host();
    void calledOnMainThread();
private:
    YADAW::Audio::Host::CLAPHost host_ {this};
    IAudioPlugin::Status status_ = IAudioPlugin::Status::Empty;
    clap_process_status processStatus_;
    const clap_plugin_entry* entry_ = nullptr;
    const clap_plugin_factory* factory_ = nullptr;
    const clap_plugin* plugin_ = nullptr;
    const clap_plugin_audio_ports* audioPorts_ = nullptr;
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
    std::unique_ptr<CLAPPluginGUI> gui_;
    std::unique_ptr<CLAPPluginParameter> parameter_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN
