#include "CLAPPlugin.hpp"

#include "audio/util/CLAPUtil.hpp"

namespace YADAW::Audio::Plugin
{
CLAPPlugin::CLAPPlugin() {}

CLAPPlugin::CLAPPlugin(const clap_plugin_entry* entry, const QString& path):
    entry_(entry)
{
    auto pathAsUtf8 = path.toUtf8();
    auto initResult = entry_->init(reinterpret_cast<const char*>(path.data()));
    if(!initResult)
    {
        entry_ = nullptr;
        return;
    }
    factory_ = reinterpret_cast<decltype(factory_)>(entry_->get_factory(CLAP_PLUGIN_FACTORY_ID));
}

bool CLAPPlugin::createPlugin(const char* id)
{
    if(factory_)
    {
        plugin_ = factory_->create_plugin(factory_, host_.host(), id); // FIXME: clap_host
        if(plugin_)
        {
            status_ = IPlugin::Status::Loaded;
        }
        return plugin_;
    }
    return false;
}

CLAPPlugin::~CLAPPlugin()
{
    if(status_ == IPlugin::Status::Processing)
    {
        CLAPPlugin::stopProcessing();
    }
    if(status_ == IPlugin::Status::Activated)
    {
        CLAPPlugin::deactivate();
    }
    if(status_ == IPlugin::Status::Initialized)
    {
        CLAPPlugin::uninitialize();
    }
    if(status_ == IPlugin::Status::Created)
    {
    }
    if(entry_)
    {
        entry_->deinit();
        entry_ = nullptr;
    }
}

const clap_plugin_entry* CLAPPlugin::entry()
{
    return entry_;
}

const clap_plugin_factory* CLAPPlugin::factory()
{
    return factory_;
}

QString CLAPPlugin::name() const
{
    return QString::fromUtf8(plugin_->desc->name);
}

bool CLAPPlugin::initialize(double sampleRate, std::int32_t maxSampleCount)
{
    if(plugin_)
    {
        plugin_->init(plugin_);
        getExtension(plugin_, CLAP_EXT_AUDIO_PORTS, &audioPorts_);
        if(!audioPorts_)
        {
            return false;
        }
        getExtension(plugin_, CLAP_EXT_LATENCY, &latency_);
        // TODO
        prepareAudioRelatedInfo();
        prepareProcessData();
        status_ = IPlugin::Status::Initialized;
        return true;
    }
    return false;
}

bool CLAPPlugin::uninitialize()
{
    gui_.reset();
    parameter_.reset();
    if(plugin_)
    {
        plugin_->destroy(plugin_);
        status_ = IPlugin::Status::Created;
    }
    return true;
}

bool CLAPPlugin::activate()
{
    if(plugin_->activate(plugin_, sampleRate_, minBlockSize_, maxBlockSize_))
    {
        status_ = IPlugin::Status::Activated;
        return true;
    }
    return false;
}

bool CLAPPlugin::deactivate()
{
    plugin_->deactivate(plugin_);
    status_ = IPlugin::Status::Initialized;
    return true;
}

bool CLAPPlugin::startProcessing()
{
    if(plugin_->start_processing(plugin_))
    {
        status_ = IPlugin::Status::Processing;
        return true;
    }
    return false;
}

bool CLAPPlugin::stopProcessing()
{
    plugin_->stop_processing(plugin_);
    status_ = IPlugin::Status::Activated;
    return true;
}

IPlugin::Format CLAPPlugin::format()
{
    return IPlugin::Format::CLAP;
}

IPlugin::Status CLAPPlugin::status()
{
    return status_;
}

IPluginGUI* CLAPPlugin::gui()
{
    if(!gui_)
    {
        const clap_plugin_gui* gui = nullptr;
        getExtension(plugin_, CLAP_EXT_GUI, &gui);
        if(gui)
        {
            gui_ = std::make_unique<CLAPPluginGUI>(plugin_, gui);
        }
    }
    return gui_.get();
}

IPluginParameter* CLAPPlugin::parameter()
{
    if(!parameter_)
    {
        const clap_plugin_params* params = nullptr;
        getExtension(plugin_,CLAP_EXT_PARAMS, &params);
        if(params)
        {
            parameter_ = std::make_unique<CLAPPluginParameter>(plugin_, params);
        }
    }
    return parameter_.get();
}

int CLAPPlugin::audioInputGroupCount() const
{
    return audioPorts_->count(plugin_, true);
}

int CLAPPlugin::audioOutputGroupCount() const
{
    return audioPorts_->count(plugin_, false);
}

const IChannelGroup* CLAPPlugin::audioInputGroupAt(int index) const
{
    return &inputChannelGroups_[index];
}

const IChannelGroup* CLAPPlugin::audioOutputGroupAt(int index) const
{
    return &outputChannelGroups_[index];
}

std::uint32_t CLAPPlugin::latencyInSamples() const
{
    return latency_? latency_->get(plugin_): 0;
}

void CLAPPlugin::process(const Device::AudioProcessData<float>& audioProcessData)
{
    // TODO: Fetch event list from host
    processData_.frames_count = audioProcessData.singleBufferSize;
    for(int i = 0; i < processData_.audio_inputs_count; ++i)
    {
        inputBuffers_[i].data32 = audioProcessData.inputs[i];
    }
    for(int i = 0; i < processData_.audio_outputs_count; ++i)
    {
        outputBuffers_[i].data32 = audioProcessData.outputs[i];
    }
    plugin_->process(plugin_, &processData_);
}

void CLAPPlugin::prepareAudioRelatedInfo()
{
    auto audioInputGroupCount = this->audioInputGroupCount();
    auto audioOutputGroupCount = this->audioOutputGroupCount();
    inputChannelGroups_.reserve(audioInputGroupCount);
    inputBuffers_.reserve(audioInputGroupCount);
    outputChannelGroups_.reserve(audioOutputGroupCount);
    outputBuffers_.reserve(audioOutputGroupCount);
    for(int i = 0; i < audioInputGroupCount; ++i)
    {
        inputChannelGroups_.emplace_back(plugin_, audioPorts_, true, i);
        inputBuffers_.emplace_back();
        inputBuffers_.back().channel_count = inputChannelGroups_[i].channelCount();
    }
    for(int i = 0; i < audioOutputGroupCount; ++i)
    {
        outputChannelGroups_.emplace_back(plugin_, audioPorts_, false, i);
        outputBuffers_.emplace_back();
        outputBuffers_.back().channel_count = outputChannelGroups_[i].channelCount();
    }
}

void CLAPPlugin::clearAudioRelatedInfo()
{
    inputBuffers_.clear();
    outputBuffers_.clear();
    inputChannelGroups_.clear();
    outputChannelGroups_.clear();
}

void CLAPPlugin::prepareProcessData()
{
    processData_.audio_inputs_count = audioInputGroupCount();
    processData_.audio_outputs_count = audioOutputGroupCount();
    processData_.audio_inputs = inputBuffers_.data();
    processData_.audio_outputs = outputBuffers_.data();
}

void CLAPPlugin::resetProcessData()
{
    processData_ = {};
}

void CLAPPlugin::calledOnMainThread()
{
    plugin_->on_main_thread(plugin_);
}
}