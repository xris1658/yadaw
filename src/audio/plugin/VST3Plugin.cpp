#include "VST3Plugin.hpp"

#include "audio/host/VST3Host.hpp"
#ifdef __linux__
#include "audio/host/VST3RunLoop.hpp"
#endif
#include "audio/plugin/VST3PluginGUI.hpp"
#include "audio/plugin/VST3PluginParameter.hpp"
#include "audio/util/VST3Helper.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

// For some reason, memorystream.cpp is not included in sdk_common library, so I have to solve this
// by `#include`ing the source file in another source: `audio/plugin/VST3MemoryStream.cpp`.
#include <public.sdk/source/common/memorystream.h>

namespace YADAW::Audio::Plugin
{
Steinberg::Vst::SpeakerArrangement vst3ChannelGroupMapping[] = {
    SpeakerArr::kEmpty,
    SpeakerArr::kMono,
    SpeakerArr::kStereo,
    SpeakerArr::k30Cine,
    SpeakerArr::k40Music,
    SpeakerArr::k50,
    SpeakerArr::k51,
    SpeakerArr::k61Cine,
    SpeakerArr::k71Cine
};

YADAW::Audio::Base::ChannelGroupType fromSpeakerArrangement(SpeakerArrangement speakerArrangement)
{
    using namespace YADAW::Audio::Base;
    using namespace SpeakerArr;
    switch(speakerArrangement)
    {
    case kEmpty:
        return ChannelGroupType::eEmpty;
    case kMono:
        return ChannelGroupType::eMono;
    case kStereo:
        return ChannelGroupType::eStereo;
    case k30Cine:
        return ChannelGroupType::eLRC;
    case k40Music:
        return ChannelGroupType::eQuad;
    case k50:
        return ChannelGroupType::e50;
    case k51:
        return ChannelGroupType::e51;
    case k61Cine:
        return ChannelGroupType::e61;
    case k71Cine:
        return ChannelGroupType::e71;
    default:
        return ChannelGroupType::eCustomGroup;
    }
}

Steinberg::Vst::SpeakerArrangement fromChannelGroup(YADAW::Audio::Base::ChannelGroupType channelGroup)
{
    if(auto index = YADAW::Util::underlyingValue(channelGroup);
        index >= 0
        && index < YADAW::Util::underlyingValue(YADAW::Audio::Base::ChannelGroupType::eEnd))
    {
        return vst3ChannelGroupMapping[index];
    }
    return SpeakerArr::kEmpty;
}

VST3Plugin::VST3Plugin()
{
}

VST3Plugin::VST3Plugin(
    YADAW::Native::VST3InitEntry initEntry,
    VST3Plugin::FactoryEntry factoryEntry,
    VST3Plugin::ExitEntry exitEntry,
    void* libraryHandle):
    exitEntry_(nullptr)
{
    if(YADAW::Native::initVST3Entry(initEntry, libraryHandle))
    {
        if(factoryEntry)
        {
            if(auto factory = factoryEntry(); factory)
            {
                factory_ = factory;
#if __linux__
                queryInterface(factory_, &factory3_);
                factory3_->setHostContext(&YADAW::Audio::Host::VST3Host::instance());
#endif
                status_ = IAudioPlugin::Status::Loaded;
            }
        }
        exitEntry_ = exitEntry;
    }
}

VST3Plugin::VST3Plugin(VST3Plugin&& rhs):
    status_(rhs.status_),
    unified_(rhs.unified_),
    exitEntry_(rhs.exitEntry_),
    factory_(rhs.factory_),
#if __linux__
    factory3_(rhs.factory3_),
#endif
    component_(rhs.component_),
    audioProcessor_(rhs.audioProcessor_),
    componentPoint_(rhs.componentPoint_),
    editControllerPoint_(rhs.editControllerPoint_),
    audioInputChannelGroup_(std::move(rhs.audioInputChannelGroup_)),
    audioOutputChannelGroup_(std::move(rhs.audioOutputChannelGroup_)),
    audioInputBusActivated_(std::move(rhs.audioInputBusActivated_)),
    audioOutputBusActivated_(std::move(rhs.audioOutputBusActivated_)),
    processSetup_(rhs.processSetup_),
    processData_(rhs.processData_),
    inputBuffers_(rhs.inputBuffers_),
    outputBuffers_(rhs.outputBuffers_),
    gui_(std::move(rhs.gui_)),
    parameter_(std::move(rhs.parameter_)),
    componentHandler_(std::move(rhs.componentHandler_)),
    eventProcessor_(std::move(rhs.eventProcessor_))
{
    rhs.status_ = YADAW::Audio::Plugin::IAudioPlugin::Status::Empty;
    rhs.exitEntry_ = nullptr;
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
#if __linux__
        releasePointer(factory3_);
#endif
        releasePointer(factory_);
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

bool VST3Plugin::setChannelGroups(YADAW::Audio::Base::ChannelGroupType* inputs, std::uint32_t inputCount,
    YADAW::Audio::Base::ChannelGroupType* outputs, std::uint32_t outputCount)
{
    std::vector<Steinberg::Vst::SpeakerArrangement> inputsAsSpeakerArrangement;
    inputsAsSpeakerArrangement.reserve(inputCount);
    std::vector<Steinberg::Vst::SpeakerArrangement> outputsAsSpeakerArrangement;
    outputsAsSpeakerArrangement.reserve(outputCount);
    for(std::uint32_t i = 0; i < inputCount; ++i)
    {
        inputsAsSpeakerArrangement.emplace_back(fromChannelGroup(inputs[i]));
    }
    for(std::uint32_t i = 0; i < outputCount; ++i)
    {
        outputsAsSpeakerArrangement.emplace_back(fromChannelGroup(outputs[i]));
    }
    auto ret = audioProcessor_->setBusArrangements(inputsAsSpeakerArrangement.data(), inputCount,
        outputsAsSpeakerArrangement.data(), outputCount);
    if(ret == Steinberg::kResultOk)
    {
        prepareAudioRelatedInfo();
        prepareProcessData(processSetup_.processMode);
        return true;
    }
    return false;
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
        Steinberg::int64 result;
        // https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/API+Documentation/Index.html#initialization-of-communication-from-host-point-of-view
        // `stream.rewind()` appeared in the above code snippet, indicating that
        // we should rewind the stream object before passing it to the
        // `IEditController` object.
        // Since `Steinberg::MemoryStream::rewind()` does NOT exist, we'll just
        // rewind it manually.
        // This is checked on initializing VST3 Host Checker. It tries to break
        // if the stream is not rewinded and the program is being debugged on
        // Linux. This problem does not exist on Windows, even if we forgot to
        // rewind the stream.
        stream.seek(0, Steinberg::IBStream::IStreamSeekMode::kIBSeekSet, &result);
        // component_->setState(&stream);
        editController_->setComponentState(&stream);
    }
    // TODO: negotiate bus arrangement
    prepareAudioRelatedInfo();
    audioInputBusActivated_.resize(audioInputGroupCount(), false);
    audioOutputBusActivated_.resize(audioOutputGroupCount(), false);
    FOR_RANGE0(i, audioInputGroupCount())
    {
        activateAudioInputGroup(i, true);
    }
    FOR_RANGE0(i, audioOutputGroupCount())
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
    auto result = audioProcessor_->setProcessing(false);
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

const IPluginGUI* VST3Plugin::gui() const
{
    return pluginGUI();
}

IAudioDeviceParameter* VST3Plugin::parameter()
{
    return pluginParameter();
}

const IAudioDeviceParameter* VST3Plugin::parameter() const
{
    return pluginParameter();
}

const VST3PluginGUI* VST3Plugin::pluginGUI() const
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
VST3PluginGUI* VST3Plugin::pluginGUI()
{
    return const_cast<VST3PluginGUI*>(
        static_cast<const VST3Plugin&>(*this).pluginGUI()
    );
}

// FIXME: Not thread-safe
const VST3PluginParameter* VST3Plugin::pluginParameter() const
{
    if(editController_ && (!parameter_))
    {
        parameter_ = std::make_unique<VST3PluginParameter>(editController_);
    }
    return parameter_.get();
}

VST3PluginParameter* VST3Plugin::pluginParameter()
{
    return const_cast<VST3PluginParameter*>(
        static_cast<const VST3Plugin&>(*this).pluginParameter()
    );
}

std::uint32_t VST3Plugin::audioInputGroupCount() const
{
    return component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kInput);
}

std::uint32_t VST3Plugin::audioOutputGroupCount() const
{
    return component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kOutput);
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
VST3Plugin::audioInputGroupAt(std::uint32_t index) const
{
    return index < audioInputGroupCount()?
        std::optional(std::cref(audioInputChannelGroup_[index])):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
VST3Plugin::audioOutputGroupAt(std::uint32_t index) const
{
    return index < audioOutputGroupCount()?
        std::optional(std::cref(audioOutputChannelGroup_[index])):
        std::nullopt;
}

bool VST3Plugin::isAudioInputGroupActivated(std::uint32_t index) const
{
    if(index < audioInputGroupCount())
    {
        return audioInputBusActivated_[index];
    }
    return false;
}

bool VST3Plugin::activateAudioInputGroup(std::uint32_t index, bool state)
{
    if(index < audioInputGroupCount())
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

bool VST3Plugin::isAudioOutputGroupActivated(std::uint32_t index) const
{
    if(index < audioOutputGroupCount())
    {
        return audioOutputBusActivated_[index];
    }
    return false;
}

bool VST3Plugin::activateAudioOutputGroup(std::uint32_t index, bool state)
{
    if(index < audioOutputGroupCount())
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
    (this->*processFunc_[status_ == IAudioPlugin::Status::Processing])(audioProcessData);
}

void VST3Plugin::doProcess(const AudioProcessData<float>& audioProcessData)
{
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

void VST3Plugin::blankProcess(const AudioProcessData<float>&) {}

Steinberg::Vst::IComponentHandler* VST3Plugin::componentHandler()
{
    return componentHandler_;
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
    FOR_RANGE0(i, audioInputGroupCount)
    {
        audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kInput, i,
            audioInputChannelGroup_[i].speakerArrangement_);
        component_->getBusInfo(Steinberg::Vst::MediaTypes::kAudio,
            Steinberg::Vst::BusDirections::kInput, i, audioInputChannelGroup_[i].busInfo_);
        inputBuffers_[i].numChannels = audioInputChannelGroup_[i].busInfo_.channelCount;
    }
    FOR_RANGE0(i, audioOutputGroupCount)
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
        if(componentHandler_)
        {
            editController_->setComponentHandler(componentHandler_);
        }
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
        editController_->setComponentHandler(nullptr);
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

void VST3Plugin::refreshParameterInfo()
{
    if(parameter_)
    {
        parameter_->refreshParameterInfo();
    }
}

void VST3Plugin::setComponentHandler(IComponentHandler& componentHandler)
{
    componentHandler_ = &componentHandler;
}

void VST3Plugin::setProcessContext(Steinberg::Vst::ProcessContext& processContext)
{
    processData_.processContext = &processContext;
}

void VST3Plugin::setParameterChanges(IParameterChanges& inputParameterChanges,
    Steinberg::Vst::IParameterChanges* outputParameterChanges)
{
    processData_.inputParameterChanges = &inputParameterChanges;
    processData_.outputParameterChanges = outputParameterChanges;
}

void VST3Plugin::setEventList(Steinberg::Vst::IEventList* inputEventList, Steinberg::Vst::IEventList* outputEventList)
{
    processData_.inputEvents = inputEventList;
    processData_.outputEvents = outputEventList;
}
}