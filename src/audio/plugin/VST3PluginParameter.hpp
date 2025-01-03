#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINPARAMETER
#define YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINPARAMETER

#include "audio/device/IParameter.hpp"
#include "audio/device/IAudioDeviceParameter.hpp"

#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <optional>
#include <vector>

namespace YADAW::Audio::Plugin
{
using YADAW::Audio::Device::IAudioDeviceParameter;
using YADAW::Audio::Device::IParameter;
class VST3Parameter: public YADAW::Audio::Device::IParameter
{
    friend class VST3PluginParameter;
    using Self = VST3Parameter;
public:
    VST3Parameter(Steinberg::Vst::IEditController* editController, std::int32_t index);
    ~VST3Parameter() noexcept = default;
public:
    std::uint32_t id() const override;
    QString name() const override;
    double minValue() const override;
    double maxValue() const override;
    double defaultValue() const override;
    double value() const override;
    double stepSize() const override;
    std::uint32_t stepCount() const override;
    QString unit() const override;
public:
    QString valueToString(double value) const override;
    double stringToValue(const QString& string) const override;
public:
    const Steinberg::Vst::ParameterInfo& getParameterInfo() const;
private:
    void refreshParameterInfo();
private:
    Steinberg::Vst::IEditController* editController_ = nullptr;
    Steinberg::Vst::ParameterInfo parameterInfo_ = {};
    std::int32_t index_ = -1;
};
class VST3PluginParameter: public YADAW::Audio::Device::IAudioDeviceParameter
{
    friend class VST3Plugin;
    using Self = VST3PluginParameter;
public:
    VST3PluginParameter(Steinberg::Vst::IEditController* editController);
    VST3PluginParameter(const Self&) = delete;
    VST3PluginParameter(Self&& rhs) = delete;
    Self& operator=(const Self&) = delete;
    Self& operator=(Self&& rhs) = delete;
    ~VST3PluginParameter() noexcept;
public:
    std::uint32_t parameterCount() const override;
    IParameter* parameter(std::uint32_t index) override;
    const IParameter* parameter(std::uint32_t index) const override;
public:
    void swap(Self& rhs) noexcept;
private:
    void refreshParameterInfo();
private:
    Steinberg::Vst::IEditController* editController_ = nullptr;
    Steinberg::Vst::IEditController2* editController2_ = nullptr;
    Steinberg::Vst::IEditControllerHostEditing* editControllerHostEditing_ = nullptr;
    std::vector<YADAW::Audio::Plugin::VST3Parameter> parameters_;
    int visibleParameterCount_ = 0;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VST3PLUGINPARAMETER
