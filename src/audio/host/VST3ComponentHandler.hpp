#ifndef YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
#define YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER

#include "audio/host/VST3ParameterChanges.hpp"

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
class VST3Plugin;
}

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

// A VST3 plugin uses a `IParameterChanges` pair for `IAudioProcessor::process`, with one being
// the input and the other being the output.
// For consistent processing, this class uses a double-buffering model. The component handler reads
// the output from plugin, calls `IEditController::setParamNormalized` for plugin GUI playback, and
// writes the input. In the meantime, the plugin reads the input from component handler by setting
// `ProcessData::inputParameterChanges` followed by `IAudioProcessor::process`, which fills the
// output.
// For lack of a clearer contract, this class can cause severe problems as follows, thus NOT ready
// for actual use.
// QUES: Detail of contract: Which end is responsible for clearing the parameter changes?
//  The component handler or the plugin? Why?
// QUES: Detail of contract: Which end is responsible for switching buffers?
class VST3ComponentHandler:
    public Steinberg::Vst::IComponentHandler
{
    using Self = VST3ComponentHandler;
public:
    VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin* plugin);
    VST3ComponentHandler(const Self&) = delete;
    VST3ComponentHandler(Self&& rhs) = delete;
    Self& operator=(const Self&) = delete;
    Self& operator=(Self&&) = delete;
    ~VST3ComponentHandler() noexcept;
public:
    tresult queryInterface(const Steinberg::TUID iid, void** obj) override;
    uint32 addRef() override;
    uint32 release() override;
private:
    int32 doBeginEdit(int bufferIndex, ParamID id);
    tresult doPerformEdit(int bufferIndex, int32 index, ParamValue normalizedValue,
        std::int64_t timestamp);
    tresult doEndEdit(ParamID id);
public:
    tresult PLUGIN_API beginEdit(ParamID id) override;
    tresult PLUGIN_API performEdit(ParamID id, ParamValue normalizedValue) override;
    tresult PLUGIN_API endEdit(ParamID id) override;
    tresult PLUGIN_API restartComponent(int32 flags) override;
public:
    void switchBuffer(std::int64_t switchTimestampInNanosecond);
    void attachToProcessData(Vst::ProcessData& processData);
    void consumeOutputParameterChanges();
    double sampleRate() const;
public:
    void reserve();
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
    // Used by UI thread: beginEdit, performEdit, endEdit, etc.
    int bufferIndex_;
    // Set on switchBuffer
    std::int64_t timestamp_;
    YADAW::Audio::Host::VST3ParameterChanges inputParameterChanges_[2];
    YADAW::Audio::Host::VST3ParameterChanges outputParameterChanges_[2];
    std::vector<std::pair<ParamID, int32>> mappings_[2];
};

}

#endif //YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
