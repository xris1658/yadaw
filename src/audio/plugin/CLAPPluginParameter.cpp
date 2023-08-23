#include "CLAPPluginParameter.hpp"

#include "util/ArrayAccess.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Plugin
{
CLAPParameter::CLAPParameter(const clap_plugin* plugin, const clap_plugin_params* params,
    std::uint32_t index):
    plugin_(plugin),
    params_(params),
    index_(index)
{
    refreshInfo();
    flags_ |= ParameterFlags::SupportMinMaxValue;
    flags_ |= ParameterFlags::SupportDefaultValue;
}

std::uint32_t CLAPParameter::id() const
{
    return paramInfo_.id;
}

QString CLAPParameter::name() const
{
    return QString::fromUtf8(paramInfo_.name);
}

double CLAPParameter::minValue() const
{
    return paramInfo_.min_value;
}

double CLAPParameter::maxValue() const
{
    return paramInfo_.max_value;
}

double CLAPParameter::defaultValue() const
{
    return paramInfo_.default_value;
}

double CLAPParameter::value() const
{
    double ret = 0;
    params_->get_value(plugin_, id(), &ret);
    return ret;
}

void CLAPParameter::setValue(double value)
{
    // TODO
}

double CLAPParameter::stepSize() const
{
    return flags_ & ParameterFlags::Discrete? 1: 0;
}

QString CLAPParameter::valueToString(double value) const
{
    constexpr std::uint32_t bufferSize = 128;
    char buffer[bufferSize];
    if(params_->value_to_text(plugin_, paramInfo_.id, value, buffer, bufferSize)
       && buffer[0] != 0)
    {
        return QString::fromUtf8(buffer);
    }
    return QString::number(value);
}

double CLAPParameter::stringToValue(const QString& string) const
{
    auto utf8 = string.toUtf8();
    double ret;
    return params_->text_to_value(plugin_, paramInfo_.id, utf8.data(), &ret)? ret: -1;
}

void CLAPParameter::refreshInfo()
{
    params_->get_info(plugin_, index_, &paramInfo_);
    auto flags = paramInfo_.flags;
    if(flags & CLAP_PARAM_IS_STEPPED)
    {
        flags_ |= ParameterFlags::Discrete;
    }
    if(flags & CLAP_PARAM_IS_PERIODIC)
    {
        flags_ |= ParameterFlags::Periodic;
    }
    if(flags & CLAP_PARAM_IS_HIDDEN)
    {
        flags_ |= ParameterFlags::Hidden;
    }
    if(flags & CLAP_PARAM_IS_READONLY)
    {
        flags_ |= ParameterFlags::Readonly;
    }
    if(flags & CLAP_PARAM_IS_AUTOMATABLE)
    {
        flags_ |= ParameterFlags::Automatable;
    }
}

CLAPPluginParameter::CLAPPluginParameter(const clap_plugin* plugin, const clap_plugin_params* params):
    plugin_(plugin),
    params_(params)
{
    auto count = parameterCount();
    parameters_.reserve(count);
    for(auto i: YADAW::Util::IntegerRange(count))
    {
        parameters_.emplace_back(plugin_, params_, i);
    }
}

CLAPPluginParameter::~CLAPPluginParameter() noexcept
{
    plugin_ = nullptr;
    params_ = nullptr;
}

std::uint32_t CLAPPluginParameter::parameterCount()
{
    return params_->count(plugin_);
}

IParameter* CLAPPluginParameter::parameter(std::uint32_t index)
{
    return YADAW::Util::getOrNull(parameters_, index);
}

void CLAPPluginParameter::swap(CLAPPluginParameter::Self& rhs) noexcept
{
    std::swap(plugin_, rhs.plugin_);
    std::swap(params_, rhs.params_);
    std::swap(parameters_, rhs.parameters_);
}

void CLAPPluginParameter::refreshParameterInfo()
{
    for(auto& parameter: parameters_)
    {
        parameter.refreshInfo();
    }
}
}