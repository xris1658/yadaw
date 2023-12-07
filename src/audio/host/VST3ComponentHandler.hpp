#ifndef YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
#define YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER

#include "audio/host/VST3ParameterChanges.hpp"
#include "util/AtomicMutex.hpp"

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <atomic>
#include <functional>
#include <vector>

namespace YADAW::Audio::Plugin
{
class VST3Plugin;
}

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

// A VST3 plugin uses a `IParameterChanges` pair for `IAudioProcessor::process`,
// with one being the input and the other being the output.
// For consistent processing, this class uses a double-buffering model. The
// component handler reads the output from plugin, calls
// `IEditController::setParamNormalized` for plugin GUI playback, and
// writes the input. In the meantime, the plugin reads the input from component
// handler by setting `ProcessData::inputParameterChanges` followed by
// `IAudioProcessor::process`, which fills the output.
class VST3ComponentHandler:
    public Steinberg::Vst::IComponentHandler
{
    using Self = VST3ComponentHandler;
public:
    VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin& plugin);
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
    int32 doBeginEdit(int hostBufferIndex, ParamID id);
    tresult doPerformEdit(int hostBufferIndex, int32 index, ParamValue normalizedValue,
        std::int64_t timestamp);
    tresult doEndEdit(ParamID id);
public:
    tresult PLUGIN_API beginEdit(ParamID id) override;
    tresult PLUGIN_API performEdit(ParamID id, ParamValue normalizedValue) override;
    tresult PLUGIN_API endEdit(ParamID id) override;
    tresult PLUGIN_API restartComponent(int32 flags) override;
public:
    void switchBuffer(std::int64_t switchTimestampInNanosecond);
    // TODO: Use timestamp
    void consumeOutputParameterChanges(std::int64_t timestampInNanosecond);
    double sampleRate() const;
public:
    void reserve();
public:
    void latencyChanged(std::function<void()>&& callback);
    void ioChanged(std::function<void()>&& callback);
    void parameterValueChanged(std::function<void()>&& callback);
    void parameterInfoChanged(std::function<void()>&& callback);
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
    std::function<void()> latencyChanged_;
    std::function<void()> ioChanged_;
    std::function<void()> parameterValueChanged_;
    std::function<void()> parameterInfoChanged_;
    // Used by host
    std::atomic<int> hostBufferIndex_;
    // Set on switchBuffer
    std::int64_t timestamp_;
    YADAW::Audio::Host::VST3ParameterChanges inputParameterChanges_[2];
    YADAW::Audio::Host::VST3ParameterChanges outputParameterChanges_[2];
    std::vector<std::pair<ParamID, int32>> mappings_[2];
};

}

#endif // YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
