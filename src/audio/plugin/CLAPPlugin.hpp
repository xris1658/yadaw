#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN
#define YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN

#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/CLAPChannelGroup.hpp"
#include "audio/plugin/CLAPPluginGUI.hpp"
#include "audio/plugin/CLAPPluginParameter.hpp"
#include "audio/plugin/IPlugin.hpp"

#include <clap/entry.h>
#include <clap/plugin.h>
#include <clap/audio-buffer.h>
#include <clap/ext/audio-ports.h>
#include <clap/ext/gui.h>
#include <clap/ext/latency.h>
#include <clap/ext/params.h>
#include <clap/factory/plugin-factory.h>

#include <QString>

#include <vector>

namespace YADAW::Audio::Plugin
{
class CLAPPlugin: public YADAW::Audio::Plugin::IPlugin
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
    IPluginGUI* gui() override;
    IPluginParameter* parameter() override;
public:
    int audioInputGroupCount() const override;
    int audioOutputGroupCount() const override;
    const IChannelGroup* audioInputGroupAt(int index) const override;
    const IChannelGroup* audioOutputGroupAt(int index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
private:
    void prepareAudioRelatedInfo();
    void clearAudioRelatedInfo();
    void prepareProcessData();
    void resetProcessData();
public:
    clap_process& processData();
    void calledOnMainThread();
private:
    YADAW::Audio::Host::CLAPHost host_ {this};
    IPlugin::Status status_ = IPlugin::Status::Empty;
    clap_process_status processStatus_;
    const clap_plugin_entry* entry_ = nullptr;
    const clap_plugin_factory* factory_ = nullptr;
    const clap_plugin* plugin_ = nullptr;
    const clap_plugin_audio_ports* audioPorts_ = nullptr;
    const clap_plugin_latency* latency_ = nullptr;
    double sampleRate_ = 0;
    std::int32_t minBlockSize_ = 1;
    std::int32_t maxBlockSize_ = INT32_MAX;
    std::vector<CLAPChannelGroup> inputChannelGroups_;
    std::vector<CLAPChannelGroup> outputChannelGroups_;
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
    std::unique_ptr<CLAPPluginGUI> gui_;
    std::unique_ptr<CLAPPluginParameter> parameter_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGIN
