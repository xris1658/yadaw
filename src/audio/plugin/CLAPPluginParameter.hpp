#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINPARAMETER
#define YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINPARAMETER

#include "audio/plugin/IPluginParameter.hpp"

#include <clap/plugin.h>
#include <clap/ext/params.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
class CLAPParameter: public IParameter
{
public:
    CLAPParameter(const clap_plugin* plugin, const clap_plugin_params* params,
        std::uint32_t index);
public:
    std::uint32_t id() const override;
    QString name() const override;
    double minValue() const override;
    double maxValue() const override;
    double defaultValue() const override;
    double value() const override;
    void setValue(double value) override;
    double stepSize() const override;
public:
    QString valueToString(double value) const override;
    double stringToValue(const QString& string) const override;
private:
    const clap_plugin* plugin_;
    const clap_plugin_params* params_;
    std::uint32_t index_;
    clap_param_info paramInfo_ = {};
};
class CLAPPluginParameter: public IPluginParameter
{
    using Self = CLAPPluginParameter;
public:
    CLAPPluginParameter(const clap_plugin* plugin, const clap_plugin_params* params);
    CLAPPluginParameter(const Self&) = delete;
    CLAPPluginParameter(Self&& rhs) = delete;
    Self& operator=(const Self&) = delete;
    Self& operator=(Self&&) = delete;
    ~CLAPPluginParameter() noexcept;
public:
    int parameterCount() override;
    IParameter& parameter(int index) override;
public:
    void swap(Self& rhs) noexcept;
private:
    const clap_plugin* plugin_;
    const clap_plugin_params* params_;
    std::vector<CLAPParameter> parameters_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_CLAPPLUGINPARAMETER
