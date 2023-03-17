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
            status_ = IPlugin::Status::Loaded;
            return true;
        }
    }
    return false;
}

VST3Plugin::~VST3Plugin()
{
    if(status_ == IPlugin::Status::Processing)
    {
        VST3Plugin::stopProcessing();
    }
    if(status_ == IPlugin::Status::Activated)
    {
        VST3Plugin::deactivate();
    }
    if(status_ == IPlugin::Status::Initialized)
    {
        VST3Plugin::uninitialize();
    }
    if(status_ == IPlugin::Status::Created)
    {
        component_->release();
        component_ = nullptr;
        status_ = IPlugin::Loaded;
    }
    if(status_ == IPlugin::Status::Loaded)
    {
        factory_->release();
        factory_ = nullptr;
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
    auto createEditControllerResult = createEditController();
    if(component_->initialize(&YADAW::Audio::Host::VST3Host::instance()) != Steinberg::kResultOk)
    {
        return false;
    }
    if(queryInterface(component_, &audioProcessor_) != Steinberg::kResultOk)
    {
        return false;
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
    status_ = IPlugin::Status::Initialized;
    // TODO: negotiate bus arrangement
    prepareAudioRelatedInfo();
    for(int i = 0; i < audioInputGroupCount(); ++i)
    {
        component_->activateBus(MediaTypes::kAudio, BusDirections::kInput, i, true);
    }
    for(int i = 0; i < audioOutputGroupCount(); ++i)
    {
        component_->activateBus(MediaTypes::kAudio, BusDirections::kOutput, i, true);
    }
    prepareProcessData(processSetup_.processMode);
    if(createEditControllerResult)
    {
        initializeEditController();
    }
    return true;
}

bool VST3Plugin::uninitialize()
{
    uninitializeEditController();
    resetProcessData();
    clearAudioRelatedInfo();
    audioProcessor_->release();
    audioProcessor_ = nullptr;
    if(component_->terminate() == Steinberg::kResultOk)
    {
        status_ = IPlugin::Status::Created;
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
        status_ = IPlugin::Status::Activated;
        return true;
    }
    return false;
}

bool VST3Plugin::deactivate()
{
    auto result = component_->setActive(false);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IPlugin::Status::Initialized;
        return true;
    }
    return false;
}

bool VST3Plugin::startProcessing()
{
    auto result = audioProcessor_->setProcessing(true);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IPlugin::Status::Processing;
        return true;
    }
    return false;
}

bool VST3Plugin::stopProcessing()
{
    auto result = audioProcessor_->setProcessing(true);
    if(result == Steinberg::kResultOk || result == Steinberg::kNotImplemented)
    {
        status_ = IPlugin::Status::Activated;
        return true;
    }
    return false;
}

IPlugin::Format VST3Plugin::format()
{
    return IPlugin::Format::VST3;
}

IPlugin::Status VST3Plugin::status()
{
    return status_;
}

// FIXME: Not thread-safe
IPluginGUI* VST3Plugin::gui()
{
    if(editController_ && (!gui_))
    {
        gui_ = std::make_unique<VST3PluginGUI>(
            editController_->createView(Steinberg::Vst::ViewType::kEditor));
    }
    return gui_.get();
}

// FIXME: Not thread-safe
IPluginParameter* VST3Plugin::parameter()
{
    if(editController_ && (!parameter_))
    {
        parameter_ = std::make_unique<VST3PluginParameter>(
            editController_
        );
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

const Device::IChannelGroup& VST3Plugin::audioInputGroupAt(int index) const
{
    return audioInputChannelGroup_[index];
}

const Device::IChannelGroup& VST3Plugin::audioOutputGroupAt(int index) const
{
    return audioOutputChannelGroup_[index];
}

void VST3Plugin::process(const Device::AudioProcessData<float>& audioProcessData)
{
    if(componentHandler_)
    {
        componentHandler_->consumeInputParameterChanges(processData_);
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

void VST3Plugin::consumeOutputParameterChanges(Steinberg::Vst::IParameterChanges* outputParameterChanges)
{
    auto outputParameterChangeCount = outputParameterChanges->getParameterCount();
    for(decltype(outputParameterChangeCount) i = 0; i < outputParameterChangeCount; ++i)
    {
        if(auto ptr = outputParameterChanges->getParameterData(i); ptr)
        {
            auto pointCount = ptr->getPointCount();
            if(pointCount != 0)
            {
                Steinberg::int32 sampleOffset = 0;
                ParamValue value = 0.0;
                if(ptr->getPoint(pointCount - 1, sampleOffset, value) == Steinberg::kResultOk)
                {
                    editController_->setParamNormalized(ptr->getParameterId(), value);
                }
            }
        }
    }
}

void VST3Plugin::prepareAudioRelatedInfo()
{
    assert(status_ == IPlugin::Status::Initialized);
    auto audioInputGroupCount = this->audioInputGroupCount();
    auto audioOutputGroupCount = this->audioOutputGroupCount();
    audioInputChannelGroup_ = std::vector<VST3ChannelGroup>(audioInputGroupCount);
    audioOutputChannelGroup_ = std::vector<VST3ChannelGroup>(audioOutputGroupCount);
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
        return factory_->createInstance(uid, Steinberg::Vst::IEditController::iid,
            reinterpret_cast<void**>(&editController_)) == Steinberg::kResultOk;
    }
    return queryInterface(component_, &editController_) == Steinberg::kResultOk;
}

bool VST3Plugin::initializeEditController()
{
    if(editController_->initialize(&YADAW::Audio::Host::VST3Host::instance()) != Steinberg::kResultOk)
    {
        return false;
    }
    componentHandler_ = std::make_unique<YADAW::Audio::Host::VST3ComponentHandler>(this);
    editController_->setComponentHandler(componentHandler_.get());
    // TODO: Replace this with self-implemented `IBStream` (not started yet)
    Steinberg::MemoryStream stream;
    if(component_->getState(&stream) == Steinberg::kResultOk)
    {
        // component_->setState(&stream);
        editController_->setComponentState(&stream);
    }
    if(queryInterface(component_, &componentPoint_) == Steinberg::kResultOk
        && queryInterface(editController_, &editControllerPoint_) == Steinberg::kResultOk
    )
    {
        componentPoint_->connect(editControllerPoint_);
        editControllerPoint_->connect(componentPoint_);
        return true;
    }
    return false;
}

bool VST3Plugin::uninitializeEditController()
{
    if(componentPoint_ && editControllerPoint_)
    {
        componentPoint_->disconnect(editControllerPoint_);
        editControllerPoint_->disconnect(componentPoint_);
        componentPoint_->release();
        componentPoint_ = nullptr;
        editControllerPoint_->release();
        editControllerPoint_ = nullptr;
    }
    if(editController_)
    {
        gui_.reset();
        auto terminateResult = editController_->terminate();
        componentHandler_.reset();
        return terminateResult == Steinberg::kResultOk;
    }
    return true;
}

bool VST3Plugin::destroyEditController()
{
    if(editController_)
    {
        editController_->release();
        editController_ = nullptr;
    }
    return true;
}
}