#include "VestifalPluginParameter.hpp"

#include "audio/util/VestifalHelper.hpp"
#include "util/ArrayAccess.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Plugin
{
VestifalParameter::VestifalParameter(AEffect* effect, std::uint32_t index):
    effect_(effect), index_(index)
{
    std::memset(&properties_, 0, sizeof(properties_));
    if(runDispatcher(effect, EffectOpcode::effectGetParameterProperties,
        index_, 0, &properties_) == 1)
    {
        IParameter::flags_ |= YADAW::Audio::Device::ParameterFlags::SupportMinMaxValue;
        if(
            properties_.flags & (
                VestifalParameterFlag::ParameterIsSwitch
                | VestifalParameterFlag::ParameterUsesFloatStepSize
                | VestifalParameterFlag::ParameterUsesIntegerStepSize
            )
        )
        {
            IParameter::flags_ |= YADAW::Audio::Device::ParameterFlags::Discrete;
        }
        if(properties_.flags & VestifalParameterFlag::ParameterIsSwitch)
        {
            IParameter::flags_ |= YADAW::Audio::Device::ParameterFlags::ShowAsSwitch;
        }
    }
}

std::uint32_t VestifalParameter::id() const
{
    return index_;
}

QString VestifalParameter::name() const
{
    char parameterName[64];
    runDispatcher(effect_, EffectOpcode::effectGetParameterName,
        index_, 0, parameterName);
    return QString::fromLocal8Bit(parameterName);
}

double VestifalParameter::minValue() const
{
    if(
        properties_.flags & (
            VestifalParameterFlag::ParameterIsSwitch
            | VestifalParameterFlag::ParameterUsesFloatStepSize
            | VestifalParameterFlag::ParameterUsesIntegerStepSize
        )
    )
    {
        return properties_.minValue;
    }
    return 0.0;
}

double VestifalParameter::maxValue() const
{
    if(
        properties_.flags & (
            VestifalParameterFlag::ParameterIsSwitch
            | VestifalParameterFlag::ParameterUsesFloatStepSize
            | VestifalParameterFlag::ParameterUsesIntegerStepSize
        )
    )
    {
        return properties_.maxValue;
    }
    return 0.0;
}

double VestifalParameter::defaultValue() const
{
    return 0;
}

double VestifalParameter::value() const
{
    return effect_->getParameter(effect_, index_);
}

void VestifalParameter::setValue(double value)
{
    effect_->setParameter(effect_, index_, value);
}

double VestifalParameter::stepSize() const
{
    if(properties_.flags & VestifalParameterFlag::ParameterUsesFloatStepSize)
    {
        return properties_.stepFloat;
    }
    if(properties_.flags & VestifalParameterFlag::ParameterUsesIntegerStepSize)
    {
        return properties_.step;
    }
    return 0.0;
}

std::uint32_t VestifalParameter::stepCount() const
{
    if(properties_.flags & VestifalParameterFlag::ParameterUsesFloatStepSize)
    {
        return (properties_.maxValue - properties_.minValue) / properties_.stepFloat;
    }
    if(properties_.flags & VestifalParameterFlag::ParameterUsesIntegerStepSize)
    {
        return (properties_.maxValue - properties_.minValue) / properties_.step;
    }
    return 0.0;
}

QString VestifalParameter::unit() const
{
    return QString();
}

QString VestifalParameter::valueToString(double value) const
{
    return QString::number(value);
}

double VestifalParameter::stringToValue(const QString& string) const
{
    bool ok = false;
    double ret[2] = {-1.0, -1.0};
    ret[1] = string.toDouble(&ok);
    return ret[ok];
}

VestifalPluginParameter::VestifalPluginParameter(AEffect* effect):
    effect_(effect)
{
    auto count = effect_->parameterCount;
    parameters_.reserve(count);
    FOR_RANGE0(i, count)
    {
        parameters_.emplace_back(effect_, i);
    }
}

std::uint32_t VestifalPluginParameter::parameterCount() const
{
    return effect_->parameterCount;
}

YADAW::Audio::Device::IParameter* VestifalPluginParameter::parameter(std::uint32_t index)
{
    return YADAW::Util::getOrNull(parameters_, index);
}

const YADAW::Audio::Device::IParameter* VestifalPluginParameter::parameter(std::uint32_t index) const
{
    return YADAW::Util::getOrNull(parameters_, index);
}
}