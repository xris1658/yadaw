#include "CLAPPlugin.hpp"

#include "audio/util/CLAPHelper.hpp"
#include "util/IntegerRange.hpp"

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
    if(factory_)
    {
        status_ = IAudioPlugin::Status::Loaded;
    }
}

bool CLAPPlugin::createPlugin(const char* id)
{
    if(factory_)
    {
        plugin_ = factory_->create_plugin(factory_, host_.host(), id);
        if(plugin_)
        {
            status_ = IAudioPlugin::Status::Created;
        }
        return plugin_;
    }
    return false;
}

CLAPPlugin::~CLAPPlugin()
{
    if(status_ == IAudioPlugin::Status::Processing)
    {
        CLAPPlugin::stopProcessing();
    }
    if(status_ == IAudioPlugin::Status::Activated)
    {
        CLAPPlugin::deactivate();
    }
    if(status_ == IAudioPlugin::Status::Initialized)
    {
        CLAPPlugin::uninitialize();
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
        auto initResult = plugin_->init(plugin_);
        if(!initResult)
        {
            plugin_->destroy(plugin_);
            status_ = IAudioPlugin::Status::Loaded;
            return false;
        }
        getExtension(plugin_, CLAP_EXT_AUDIO_PORTS, &audioPorts_);
        if(!audioPorts_)
        {
            return false;
        }
        getExtension(plugin_, CLAP_EXT_NOTE_PORTS, &notePorts_);
        if(notePorts_)
        {
            eventProcessor_ = std::make_unique<CLAPEventProcessor>(*plugin_, *notePorts_);
        }
        getExtension(plugin_, CLAP_EXT_LATENCY, &latency_);
        getExtension(plugin_, CLAP_EXT_TAIL, &tail_);
        sampleRate_ = sampleRate;
        maxBlockSize_ = maxSampleCount;
        // TODO
        prepareAudioRelatedInfo();
        prepareProcessData();
        status_ = IAudioPlugin::Status::Initialized;
        return true;
    }
    return false;
}

bool CLAPPlugin::uninitialize()
{
    eventProcessor_.reset();
    gui_.reset();
    parameter_.reset();
    status_ = IAudioPlugin::Status::Created;
    if(plugin_)
    {
        plugin_->destroy(plugin_);
        status_ = IAudioPlugin::Status::Loaded;
    }
    return true;
}

bool CLAPPlugin::activate()
{
    if(plugin_->activate(plugin_, sampleRate_, minBlockSize_, maxBlockSize_))
    {
        status_ = IAudioPlugin::Status::Activated;
        return true;
    }
    return false;
}

bool CLAPPlugin::deactivate()
{
    plugin_->deactivate(plugin_);
    status_ = IAudioPlugin::Status::Initialized;
    return true;
}

bool CLAPPlugin::startProcessing()
{
    if(plugin_->start_processing(plugin_))
    {
        status_ = IAudioPlugin::Status::Processing;
        return true;
    }
    return false;
}

bool CLAPPlugin::stopProcessing()
{
    plugin_->stop_processing(plugin_);
    status_ = IAudioPlugin::Status::Activated;
    return true;
}

IAudioPlugin::Format CLAPPlugin::format()
{
    return IAudioPlugin::Format::CLAP;
}

IAudioPlugin::Status CLAPPlugin::status()
{
    return status_;
}

std::uint32_t CLAPPlugin::tailSizeInSamples()
{
    if(tail_)
    {
        auto ret = tail_->get(plugin_);
        if(ret >= INT32_MAX)
        {
            return IAudioPlugin::InfiniteTailSize;
        }
        return ret;
    }
    return 0;
}

const IPluginGUI* CLAPPlugin::gui() const
{
    return pluginGUI();
}

IPluginGUI* CLAPPlugin::gui()
{
    return pluginGUI();
}

const IPluginParameter* CLAPPlugin::parameter() const
{
    return pluginParameter();
}

IPluginParameter* CLAPPlugin::parameter()
{
    return pluginParameter();
}

const CLAPPluginGUI* CLAPPlugin::pluginGUI() const
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

CLAPPluginGUI* CLAPPlugin::pluginGUI()
{
    return const_cast<CLAPPluginGUI*>(
        static_cast<const CLAPPlugin&>(*this).pluginGUI()
    );
}

const CLAPPluginParameter* CLAPPlugin::pluginParameter() const
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

CLAPPluginParameter* CLAPPlugin::pluginParameter()
{
    return const_cast<CLAPPluginParameter*>(
        static_cast<const CLAPPlugin&>(*this).pluginParameter()
    );
}

std::uint32_t CLAPPlugin::audioInputGroupCount() const
{
    return audioPorts_->count(plugin_, true);
}

std::uint32_t CLAPPlugin::audioOutputGroupCount() const
{
    return audioPorts_->count(plugin_, false);
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
CLAPPlugin::audioInputGroupAt(std::uint32_t index) const
{
    return index < audioInputGroupCount()?
       std::optional(std::cref(inputChannelGroups_[index])):
       std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
CLAPPlugin::audioOutputGroupAt(std::uint32_t index) const
{
    return index < audioOutputGroupCount()?
        std::optional(std::cref(outputChannelGroups_[index])):
        std::nullopt;
}

std::uint32_t CLAPPlugin::latencyInSamples() const
{
    return latency_? latency_->get(plugin_): 0;
}

void CLAPPlugin::process(const Device::AudioProcessData<float>& audioProcessData)
{
    (this->*processFunc_[status_ == IAudioPlugin::Status::Processing])(audioProcessData);
}

void CLAPPlugin::doProcess(const AudioProcessData<float>& audioProcessData)
{
    processData_.frames_count = audioProcessData.singleBufferSize;
    FOR_RANGE0(i, processData_.audio_inputs_count)
    {
        inputBuffers_[i].data32 = audioProcessData.inputs[i];
    }
    FOR_RANGE0(i, processData_.audio_outputs_count)
    {
        outputBuffers_[i].data32 = audioProcessData.outputs[i];
    }
    processStatus_ = plugin_->process(plugin_, &processData_);
}

void CLAPPlugin::blankProcess(const AudioProcessData<float>& audioProcessData)
{}

void CLAPPlugin::prepareAudioRelatedInfo()
{
    auto audioInputGroupCount = this->audioInputGroupCount();
    auto audioOutputGroupCount = this->audioOutputGroupCount();
    inputChannelGroups_.reserve(audioInputGroupCount);
    inputBuffers_.reserve(audioInputGroupCount);
    outputChannelGroups_.reserve(audioOutputGroupCount);
    outputBuffers_.reserve(audioOutputGroupCount);
    FOR_RANGE0(i, audioInputGroupCount)
    {
        inputChannelGroups_.emplace_back(plugin_, audioPorts_, true, i);
        inputBuffers_.emplace_back();
        inputBuffers_.back().channel_count = inputChannelGroups_[i].channelCount();
    }
    FOR_RANGE0(i, audioOutputGroupCount)
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

void CLAPPlugin::refreshAllParameter()
{
    const clap_plugin_params* params = nullptr;
    getExtension(plugin_, CLAP_EXT_PARAMS, &params);
    // parameter_ = std::make_unique<CLAPPluginParameter>(plugin_, params);
     *parameter_ = std::move(CLAPPluginParameter(plugin_, params));
}

void CLAPPlugin::refreshParameterInfo()
{
    if(parameter_)
    {
        parameter_->refreshParameterInfo();
    }
}

CLAPEventProcessor* CLAPPlugin::eventProcessor()
{
    return eventProcessor_.get();
}

clap_process& CLAPPlugin::processData()
{
    return processData_;
}

YADAW::Audio::Host::CLAPHost& CLAPPlugin::host()
{
    return host_;
}

void CLAPPlugin::calledOnMainThread()
{
    plugin_->on_main_thread(plugin_);
}
}