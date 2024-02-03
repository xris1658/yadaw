#include "VST3PluginParameter.hpp"

#include "audio/plugin/IParameter.hpp"
#include "audio/util/VST3Helper.hpp"
#include "util/ArrayAccess.hpp"

#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <algorithm>

namespace YADAW::Audio::Plugin
{
using namespace Steinberg::Vst;

VST3Parameter::VST3Parameter(
    Steinberg::Vst::IEditController* editController,
    std::int32_t index):
    editController_(editController),
    index_(index)
{
    editController_->addRef();
    refreshParameterInfo();
    IParameter::flags_ |= ParameterFlags::SupportMinMaxValue;
    IParameter::flags_ |= ParameterFlags::SupportDefaultValue;
}

std::uint32_t VST3Parameter::id() const
{
    return parameterInfo_.id;
}

QString VST3Parameter::name() const
{
    return QString::fromUtf16(parameterInfo_.title);
}

double VST3Parameter::minValue() const
{
    return editController_->normalizedParamToPlain(parameterInfo_.id, 0.0);
}

double VST3Parameter::maxValue() const
{
    return editController_->normalizedParamToPlain(parameterInfo_.id, 1.0);
}

double VST3Parameter::defaultValue() const
{
    return editController_->normalizedParamToPlain(parameterInfo_.id, parameterInfo_.defaultNormalizedValue);
}

double VST3Parameter::value() const
{
    auto normalizedValue = editController_->getParamNormalized(parameterInfo_.id);
    return parameterInfo_.stepCount?
        editController_->normalizedParamToPlain(parameterInfo_.id, normalizedValue):
        normalizedValue;
}

void VST3Parameter::setValue(double value)
{
    // FIXME
    if(auto stepCount = parameterInfo_.stepCount)
    {
        editController_->setParamNormalized(
            parameterInfo_.id,
            editController_->plainParamToNormalized(parameterInfo_.id, value)
        );
    }
    editController_->setParamNormalized(parameterInfo_.id, value);
}

double VST3Parameter::stepSize() const
{
    return parameterInfo_.stepCount == 0?
           parameterInfo_.stepCount:
           (maxValue() - minValue()) / static_cast<double>(parameterInfo_.stepCount);
}

QString VST3Parameter::valueToString(double value) const
{
    auto normalizedValue = editController_->plainParamToNormalized(parameterInfo_.id, value);
    String128 string;
    if(editController_->getParamStringByValue(parameterInfo_.id, normalizedValue, string) == Steinberg::kResultOk
       && string[0] != 0)
    {
        return QString::fromUtf16(string);
    }
    return QString::number(value);
}

double VST3Parameter::stringToValue(const QString& string) const
{
    ParamValue ret = 0;
    auto getValueResult = editController_->getParamValueByString(parameterInfo_.id, const_cast<Steinberg::Vst::TChar*>(reinterpret_cast<const Steinberg::Vst::TChar*>(string.data())), ret);
    return getValueResult == Steinberg::kResultOk? ret: -1;
}

const Steinberg::Vst::ParameterInfo& VST3Parameter::getParameterInfo() const
{
    return parameterInfo_;
}

void VST3Parameter::refreshParameterInfo()
{
    editController_->getParameterInfo(index_, parameterInfo_);
    if(parameterInfo_.stepCount != 0)
    {
        IParameter::flags_ |= ParameterFlags::Discrete;
    }
    if(parameterInfo_.flags & Steinberg::Vst::ParameterInfo::ParameterFlags::kIsWrapAround)
    {
        IParameter::flags_ |= ParameterFlags::Periodic;
    }
    if(parameterInfo_.flags & Steinberg::Vst::ParameterInfo::ParameterFlags::kIsHidden)
    {
        IParameter::flags_ |= ParameterFlags::Hidden;
    }
    if(parameterInfo_.flags & Steinberg::Vst::ParameterInfo::ParameterFlags::kIsReadOnly)
    {
        IParameter::flags_ |= ParameterFlags::Readonly;
    }
    if(parameterInfo_.flags & Steinberg::Vst::ParameterInfo::ParameterFlags::kCanAutomate)
    {
        IParameter::flags_ |= ParameterFlags::Automatable;
    }
    if(parameterInfo_.flags & Steinberg::Vst::ParameterInfo::ParameterFlags::kIsList)
    {
        IParameter::flags_ |= ParameterFlags::ShowAsList;
    }
}

VST3PluginParameter::VST3PluginParameter(Steinberg::Vst::IEditController* editController):
    editController_(editController)
{
    editController_->addRef();
    queryInterface(editController_, &editController2_);
    queryInterface(editController_, &editControllerHostEditing_);
    auto parameterCount = editController_->getParameterCount();
    parameters_.reserve(parameterCount);
    for(auto i = 0; i < parameterCount; ++i)
    {
        parameters_.emplace_back(editController_, i);
    }
    visibleParameterCount_ = std::distance(std::remove_if(parameters_.begin(), parameters_.end(),
        [](const VST3Parameter& parameter) { return parameter.flags() & ParameterFlags::Hidden; }
    ), parameters_.end());
}

VST3PluginParameter::~VST3PluginParameter() noexcept
{
    if(editController2_)
    {
        editController2_->release();
        editController2_ = nullptr;
    }
    if(editControllerHostEditing_)
    {
        editControllerHostEditing_->release();
        editControllerHostEditing_ = nullptr;
    }
    editController_->release();
    editController_ = nullptr;
}

std::uint32_t VST3PluginParameter::parameterCount() const
{
    return parameters_.size();
}

IParameter* VST3PluginParameter::parameter(std::uint32_t index)
{
    return YADAW::Util::getOrNull(parameters_, index);
}

const IParameter* VST3PluginParameter::parameter(std::uint32_t index) const
{
    return YADAW::Util::getOrNull(parameters_, index);
}

void VST3PluginParameter::swap(VST3PluginParameter& rhs) noexcept
{
    std::swap(editController_, rhs.editController_);
    std::swap(editController2_, rhs.editController2_);
    std::swap(editControllerHostEditing_, rhs.editControllerHostEditing_);
    std::swap(parameters_, rhs.parameters_);
}

void VST3PluginParameter::refreshParameterInfo()
{
    for(auto& parameter: parameters_)
    {
        parameter.refreshParameterInfo();
    }
}
}