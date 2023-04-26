#include "VST3Plugin.hpp"

#include "audio/host/VST3Host.hpp"
#include "audio/plugin/VST3PluginGUI.hpp"
#include "audio/plugin/VST3PluginParameter.hpp"
#include "audio/util/VST3Util.hpp"

// For some reason, memorystream.cpp is not included in sdk_common library, so I have to solve this
// by `#include`ing the source file in another source: `audio/plugin/VST3MemoryStream.cpp`.
#include <public.sdk/source/common/memorystream.h>

namespace YADAW::Audio::Plugin
{
VST3Plugin::VST3Plugin()
{
}

VST3Plugin::VST3Plugin(
    VST3Plugin::InitEntry initEntry,
    VST3Plugin::FactoryEntry factoryEntry,
    VST3Plugin::ExitEntry exitEntry):
    exitEntry_(exitEntry)
{
    if(initEntry)
    {
        if(!initEntry())
        {
            return;
        }
    }
    if(factoryEntry)
    {
        factory_ = factoryEntry();
        status_ = IAudioPlugin::Status::Loaded;
    }
}

bool VST3Plugin::createPlugin(const Steinberg::TUID& uid)
{
    if(factory_)
    {
        if(factory_->createInstance(uid, Steinberg::Vst::IComponent::iid,
            reinterpret_cast<void**>(&component_))
            == Steinberg::kResultOk)
        {
            status_ = IAudioPlugin::Status::Created;
            return true;
        }
    }
    return false;
}

void VST3Plugin::destroyPlugin()
{
    if(component_)
    {
        releasePointer(component_);
        status_ = IAudioPlugin::Loaded;
    }
}

VST3Plugin::~VST3Plugin()
{
    if(status_ == IAudioPlugin::Status::Processing)
    {
        VST3Plugin::stopProcessing();
    }
    if(status_ == IAudioPlugin::Status::Activated)
    {
        VST3Plugin::deactivate();
    }
    if(status_ == IAudioPlugin::Status::Initialized)
    {
        VST3Plugin::uninitialize();
    }
    if(status_ == IAudioPlugin::Status::Created)
    {
        destroyPlugin();
    }
    if(status_ == IAudioPlugin::Status::Loaded)
    {
        releasePointer(factory_);
        componentHandler_.reset();
    }
    if(exitEntry_)
    {
        exitEntry_();
    }
}

Steinberg::IPluginFactory* VST3Plugin::factory()
{
    return factory_;
}

IComponent* VST3Plugin::component()
{
    return component_;
}

Steinberg::Vst::IAudioProcessor* VST3Plugin::audioProcessor()
{
    return audioProcessor_;
}

Steinberg::Vst::IEditController* VST3Plugin::editController()
{
    return editController_;
}

const Steinberg::Vst::ProcessSetup& VST3Plugin::processSetup()
{
    return processSetup_;
}

bool VST3Plugin::initialize(double sampleRate, std::int32_t maxSampleCount)
{
    if(component_->initialize(&YADAW::Audio::Host::VST3Host::instance()) != Steinberg::kResultOk)
    {
        return false;
    }
    createEditController();
    initializeEditController();
    if(queryInterface(component_, &audioProcessor_) != Steinberg::kResultOk)
    {
        return false;
    }
    if(queryInterface(component_, &componentPoint_) == Steinberg::kResultOk && editController_)
    {
        componentPoint_->connect(editControllerPoint_);
        editControllerPoint_->connect(componentPoint_);
        componentHandler_->reserve();
    }
    if(audioProcessor_->canProcessSampleSize(Steinberg::Vst::SymbolicSampleSizes::kSample32) != Steinberg::kResultTrue)
    {
        return false;
    }
    processSetup_.sampleRate = sampleRate;
    processSetup_.maxSamplesPerBlock = maxSampleCount;
    processSetup_.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32;
    processSetup_.processMode = Steinberg::Vst::ProcessModes::kRealtime;
    if(audioProcessor_->setupProcessing(processSetup_) != Steinberg::kResultOk)
    {
        return false;
    }
    status_ = IAudioPlugin::Status::Initialized;
    eventProcessor_ = std::make_unique<VST3EventProcessor>(*component_);
    // TODO: Replace this with self-implemented `IBStream` (not started yet)
    Steinberg::MemoryStream stream;
    if((component_->getState(&stream) == Steinberg::kResultOk) && editController_)
    {
        // component_->setState(&stream);
        editController_->setComponentState(&stream);
    }
    // TODO: negotiate bus arrangement
    prepareAudioRelatedInfo();
    audioInputBusActivated_.resize(audioInputGroupCount(), false);
    audioOutputBusActivated_.resize(audioOutputGroupCount(), false);
    for(int i = 0; i < audioInputGroupCount(); ++i)
    {
        activateAudioInputGroup(i, true);
    }
    for(int i = 0; i < audioOutputGroupCount(); ++i)
    {
        activateAudioOutputGroup(i, true);
    }
    prepareProcessData(processSetup_.processMode);
    return true;
}

bool VST3Plugin::uninitialize()
{
    if(componentPoint_ && editControllerPoint_)
    {
        componentPoint_->disconnect(editControllerPoint_);
        editControllerPoint_->disconnect(componentPoint_);
    }
    if(editControllerPoint_)
    {
        releasePointer(editControllerPoint_);
    }
    if(componentPoint_)
    {
        releasePointer(componentPoint_);
    }
    eventProcessor_.reset();
    resetProcessData();
    clearAudioRelatedInfo();
    releasePointer(audioProcessor_);
    uninitializeEditController();
    if(component_->terminate() == Steinberg::kResultOk)
    {
        status_ = IAudioPlugin::Status::Created;
        audioInputBusActivated_.clear();
        audioOutputBusActivated_.clear();
        status_ = IAudioPlugin::Status::Created;
        destroyEditController();
        return true;
    }
    return false;
}

bool VST3Plugin::activate()
{
    auto result = component_->setActive(true);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IAudioPlugin::Status::Activated;
        return true;
    }
    return false;
}

bool VST3Plugin::deactivate()
{
    auto result = component_->setActive(false);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IAudioPlugin::Status::Initialized;
        return true;
    }
    return false;
}

bool VST3Plugin::startProcessing()
{
    auto result = audioProcessor_->setProcessing(true);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IAudioPlugin::Status::Processing;
        return true;
    }
    return false;
}

bool VST3Plugin::stopProcessing()
{
    auto result = audioProcessor_->setProcessing(true);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IAudioPlugin::Status::Activated;
        return true;
    }
    return false;
}

IAudioPlugin::Format VST3Plugin::format()
{
    return IAudioPlugin::Format::VST3;
}

IAudioPlugin::Status VST3Plugin::status()
{
    return status_;
}

std::uint32_t VST3Plugin::tailSizeInSamples()
{
    auto ret = audioProcessor_->getTailSamples();
    if(ret == Steinberg::Vst::kInfiniteTail)
    {
        return IAudioPlugin::InfiniteTailSize;
    }
    return ret;
}

IPluginGUI* VST3Plugin::gui()
{
    return pluginGUI();
}

IPluginParameter* VST3Plugin::parameter()
{
    return pluginParameter();
}

// FIXME: Not thread-safe
VST3PluginGUI* VST3Plugin::pluginGUI()
{
    if(editController_ && (!gui_))
    {
        try
        {
            if(auto plugView = editController_->createView(Steinberg::Vst::ViewType::kEditor); plugView)
            {
                gui_ = std::make_unique<VST3PluginGUI>(plugView);
            }
        }
        catch(...)
        {}
    }
    return gui_.get();
}

// FIXME: Not thread-safe
VST3PluginParameter* VST3Plugin::pluginParameter()
{
    if(editController_ && (!parameter_))
    {
        parameter_ = std::make_unique<VST3PluginParameter>(editController_);
    }
    return parameter_.get();
}

int VST3Plugin::audioInputGroupCount() const
{
    return component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kInput);
}

int VST3Plugin::audioOutputGroupCount() const
{
    return component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kOutput);
}

const IAudioChannelGroup* VST3Plugin::audioInputGroupAt(int index) const
{
    return &audioInputChannelGroup_[index];
}

const IAudioChannelGroup* VST3Plugin::audioOutputGroupAt(int index) const
{
    return &audioOutputChannelGroup_[index];
}

bool VST3Plugin::isAudioInputGroupActivated(int index) const
{
    if(index >= 0 && index < audioInputGroupCount())
    {
        return audioInputBusActivated_[index];
    }
    return false;
}

bool VST3Plugin::activateAudioInputGroup(int index, bool state)
{
    if(index >= 0 && index < audioInputGroupCount())
    {
        if(component_->activateBus(MediaTypes::kAudio, BusDirections::kInput, index, state) == Steinberg::kResultOk)
        {
            audioInputBusActivated_[index] = state;
            return true;
        }
        return false;
    }
    return false;
}

bool VST3Plugin::isAudioOutputGroupActivated(int index) const
{
    if(index >= 0 && index < audioOutputGroupCount())
    {
        return audioOutputBusActivated_[index];
    }
    return false;
}

bool VST3Plugin::activateAudioOutputGroup(int index, bool state)
{
    if(index >= 0 && index < audioOutputGroupCount())
    {
        if(component_->activateBus(MediaTypes::kAudio, BusDirections::kOutput, index, state) == Steinberg::kResultOk)
        {
            audioOutputBusActivated_[index] = state;
            return true;
        }
        return false;
    }
    return false;
}

std::uint32_t VST3Plugin::latencyInSamples() const
{
    return audioProcessor_? audioProcessor_->getLatencySamples(): 0;
}

void VST3Plugin::process(const Device::AudioProcessData<float>& audioProcessData)
{
    if(componentHandler_)
    {
        componentHandler_->attachToProcessData(processData_);
    }
    processData_.numSamples = audioProcessData.singleBufferSize;
    for(int i = 0; i < processData_.numInputs; ++i)
    {
        processData_.inputs[i].channelBuffers32 = audioProcessData.inputs[i];
    }
    for(int i = 0; i < processData_.numOutputs; ++i)
    {
        processData_.outputs[i].channelBuffers32 = audioProcessData.outputs[i];
    }
    audioProcessor_->process(processData_);
}

YADAW::Audio::Host::VST3ComponentHandler* VST3Plugin::componentHandler()
{
    return componentHandler_.get();
}

YADAW::Audio::Plugin::VST3EventProcessor* VST3Plugin::eventProcessor()
{
    return eventProcessor_.get();
}

void VST3Plugin::prepareAudioRelatedInfo()
{
    assert(status_ == IAudioPlugin::Status::Initialized);
    auto audioInputGroupCount = this->audioInputGroupCount();
    auto audioOutputGroupCount = this->audioOutputGroupCount();
    audioInputChannelGroup_ = std::vector<VST3AudioChannelGroup>(audioInputGroupCount);
    audioOutputChannelGroup_ = std::vector<VST3AudioChannelGroup>(audioOutputGroupCount);
    inputBuffers_ = std::vector<Steinberg::Vst::AudioBusBuffers>(audioInputGroupCount);
    outputBuffers_ = std::vector<Steinberg::Vst::AudioBusBuffers>(audioOutputGroupCount);
    for(int i = 0; i < audioInputGroupCount; ++i)
    {
        audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kInput, i,
            audioInputChannelGroup_[i].speakerArrangement_);
        component_->getBusInfo(Steinberg::Vst::MediaTypes::kAudio,
            Steinberg::Vst::BusDirections::kInput, i, audioInputChannelGroup_[i].busInfo_);
        inputBuffers_[i].numChannels = audioInputChannelGroup_[i].busInfo_.channelCount;
    }
    for(int i = 0; i < audioOutputGroupCount; ++i)
    {
        audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kOutput, i,
            audioOutputChannelGroup_[i].speakerArrangement_);
        component_->getBusInfo(Steinberg::Vst::MediaTypes::kAudio,
            Steinberg::Vst::BusDirections::kOutput, i, audioOutputChannelGroup_[i].busInfo_);
        outputBuffers_[i].numChannels = audioOutputChannelGroup_[i].busInfo_.channelCount;
    }
}

void VST3Plugin::clearAudioRelatedInfo()
{
    inputBuffers_.clear();
    outputBuffers_.clear();
    audioInputChannelGroup_.clear();
    audioOutputChannelGroup_.clear();
}

void VST3Plugin::prepareProcessData(Steinberg::int32 processMode)
{
    processData_.processMode = processMode;
    processData_.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32;
    processData_.numInputs = audioInputGroupCount();
    processData_.numOutputs = audioOutputGroupCount();
    processData_.inputs = inputBuffers_.data();
    processData_.outputs = outputBuffers_.data();
}

void VST3Plugin::resetProcessData()
{
    processData_ = {};
}

bool VST3Plugin::createEditController()
{
    Steinberg::TUID uid;
    if(component_->getControllerClassId(uid) == Steinberg::kResultOk)
    {
        if(factory_->createInstance(uid, Steinberg::Vst::IEditController::iid,
            reinterpret_cast<void**>(&editController_)) == Steinberg::kResultOk)
        {
            return true;
        }
    }
    if(!editController_)
    {
        if(auto queryInterfaceResult = queryInterface(component_, &editController_);
            queryInterfaceResult == Steinberg::kResultOk)
        {
            unified_  = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool VST3Plugin::initializeEditController()
{
    if(editController_)
    {
        if(unified_ == 0)
        {
            if(auto initializeEditControllerResult = editController_->initialize(
                    &YADAW::Audio::Host::VST3Host::instance());
                initializeEditControllerResult != Steinberg::kResultOk)
            {
                return false;
            }
        }
        componentHandler_ = std::make_unique<YADAW::Audio::Host::VST3ComponentHandler>(this);
        editController_->setComponentHandler(componentHandler_.get());
        queryInterface(editController_, &editControllerPoint_);
        return true;
    }
    return false;
}

bool VST3Plugin::uninitializeEditController()
{
    if(editController_)
    {
        parameter_.reset();
        gui_.reset();
        if(!unified_)
        {
            auto terminateEditControllerResult = editController_->terminate();
            return terminateEditControllerResult == Steinberg::kResultOk;
        }
        unified_ = 0;
    }
    return true;
}

bool VST3Plugin::destroyEditController()
{
    if(editController_)
    {
        releasePointer(editController_);
    }
    return true;
}

void VST3Plugin::setProcessContext(Steinberg::Vst::ProcessContext* processContext)
{
    processData_.processContext = processContext;
}
}