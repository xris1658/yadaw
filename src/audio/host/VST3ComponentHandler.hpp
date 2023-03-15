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
    DECLARE_FUNKNOWN_METHODS
private:
    int32 doBeginEdit(int bufferIndex, ParamID id);
    tresult doPerformEdit(int bufferIndex, int32 index, ParamValue normalizedValue,
        std::int64_t timestamp);
public:
    tresult PLUGIN_API beginEdit(ParamID id) override;
    tresult PLUGIN_API performEdit(ParamID id, ParamValue normalizedValue) override;
    tresult PLUGIN_API endEdit(ParamID id) override;
    tresult PLUGIN_API restartComponent(int32 flags) override;
public:
    void switchBuffer(std::int64_t switchTimestampInNanosecond);
    void consumeParameterChanges(Vst::ProcessData& processData);
    double sampleRate() const;
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
    // Used by UI thread: beginEdit, performEdit, endEdit, etc.
    int bufferIndex_;
    // Set on switchBuffer
    std::int64_t timestamp_;
    YADAW::Audio::Host::VST3ParameterChanges parameterChanges_[2];
    std::vector<std::pair<ParamID, int32>> mappings_[2];
};

}

#endif //YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
