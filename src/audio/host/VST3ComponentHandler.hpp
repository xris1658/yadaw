#ifndef YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
#define YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER

#include "audio/host/VST3ParameterChanges.hpp"
#include "concurrent/AtomicMutex.hpp"

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <QTimer>

#include <atomic>
#include <forward_list>
#include <functional>
#include <map>
#include <memory>
#include <set>
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
class VST3ComponentHandler final:
    public Steinberg::Vst::IComponentHandler
{
    using Self = VST3ComponentHandler;
public:
    using LatencyChangedCallback = void(YADAW::Audio::Plugin::VST3Plugin&);
    using IOChangedCallback = void(YADAW::Audio::Plugin::VST3Plugin&);
    using ParameterValueChangedCallback = void(YADAW::Audio::Plugin::VST3Plugin&);
    using ParameterInfoChangedCallback = void(YADAW::Audio::Plugin::VST3Plugin&);
    using ParameterBeginEditCallback = void(YADAW::Audio::Plugin::VST3Plugin& plugin, ParamID id, std::int64_t timestampInNanosecond);
    using ParameterPerformEditCallback = void(YADAW::Audio::Plugin::VST3Plugin& plugin, ParamID id, ParamValue normalizedValue, std::int64_t timestampInNanosecond);
    using ParameterEndEditCallback = void(YADAW::Audio::Plugin::VST3Plugin& plugin, ParamID id, std::int64_t timestampInNanosecond);
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
    int32 doBeginEdit(int hostBufferIndex, ParamID id, std::int64_t timestampInNanosecond);
    tresult doPerformEdit(int hostBufferIndex, int32 index, ParamValue normalizedValue,
        std::int64_t timestampInNanosecond);
    tresult doEndEdit(ParamID id, std::int64_t timestampInNanosecond);
public:
    tresult PLUGIN_API beginEdit(ParamID id) override;
    tresult PLUGIN_API performEdit(ParamID id, ParamValue normalizedValue) override;
    tresult PLUGIN_API endEdit(ParamID id) override;
    tresult PLUGIN_API restartComponent(int32 flags) override;
public:
    void bufferSwitched(std::int64_t switchTimestampInNanosecond);
    // TODO: Use timestamp
    void consumeOutputParameterChanges(std::int64_t timestampInNanosecond);
    double sampleRate() const;
public:
    void reserve();
public:
    void setLatencyChangedCallback(LatencyChangedCallback* callback);
    void setIOChangedCallback(IOChangedCallback* callback);
    void setParameterValueChangedCallback(ParameterValueChangedCallback* callback);
    void setParameterInfoChangedCallback(ParameterInfoChangedCallback* callback);
    void setParameterBeginEditCallback(ParameterBeginEditCallback* callback);
    void setParameterPerformEditCallback(ParameterPerformEditCallback* callback);
    void setParameterEndEditCallback(ParameterEndEditCallback* callback);
    void resetLatencyChangedCallback();
    void resetIOChangedCallback();
    void resetParameterValueChangedCallback();
    void resetParameterInfoChangedCallback();
    void resetParameterBeginEditCallback();
    void resetParameterPerformEditCallback();
    void resetParameterEndEditCallback();
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
    LatencyChangedCallback* latencyChangedCallback_;
    IOChangedCallback* ioChangedCallback_;
    ParameterValueChangedCallback* parameterValueChangedCallback_;
    ParameterInfoChangedCallback* parameterInfoChangedCallback_;
    ParameterBeginEditCallback* parameterBeginEditCallback_;
    ParameterPerformEditCallback* parameterPerformEditCallback_;
    ParameterEndEditCallback* parameterEndEditCallback_;

    // Set on switchBuffer
    std::int64_t timestamp_;
    std::map<ParamID, std::unique_ptr<QTimer>> editingParameters_;
    std::forward_list<std::unique_ptr<QTimer>> availableTimers_;
    YADAW::Audio::Host::VST3ParameterChanges inputParameterChanges_[2];
    YADAW::Audio::Host::VST3ParameterChanges outputParameterChanges_[2];
    std::vector<std::pair<ParamID, int32>> mappings_[2];
};

}

#endif // YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
