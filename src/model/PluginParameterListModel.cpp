#include "PluginParameterListModel.hpp"

namespace YADAW::Model
{
PluginParameterListModel::PluginParameterListModel(
    YADAW::Audio::Plugin::IPluginParameter& pluginParameter, QObject* parent):
    IPluginParameterListModel(parent),
    pluginParameter_(&pluginParameter)
{}

PluginParameterListModel::~PluginParameterListModel()
{}

int PluginParameterListModel::itemCount() const
{
    return pluginParameter_->parameterCount();
}

int PluginParameterListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant PluginParameterListModel::data(const QModelIndex& index, int role) const
{
    using namespace YADAW::Audio::Plugin;
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto parameter = pluginParameter_->parameter(row);
        if(parameter)
        {
            switch(role)
            {
            case Role::Id:
            {
                auto id = parameter->id();
                // The returned value is converted from `unsigned int` (which is
                // not supported in QML) into `int`.
                // It might be better to use `Number` from JS, since every
                // `unsigned int` can be represented without loss.
                int signedId;
                std::memcpy(&signedId, &id, sizeof(int));
                return QVariant::fromValue<int>(signedId);
            }
            case Role::Name:
                return parameter->name();
            case Role::MinValue:
                return parameter->minValue();
            case Role::MaxValue:
                return parameter->maxValue();
            case Role::DefaultValue:
                return parameter->defaultValue();
            case Role::Value:
                return parameter->value();
            case Role::StepSize:
                return parameter->stepSize();
            case Role::IsDiscrete:
                return parameter->flags() & ParameterFlags::Discrete;
            case Role::IsPeriodic:
                return parameter->flags() & ParameterFlags::Periodic;
            case Role::IsVisible:
                return (parameter->flags() & ParameterFlags::Hidden) != 0;
            case Role::IsReadonly:
                return parameter->flags() & ParameterFlags::Readonly;
            case Role::IsAutomatable:
                return parameter->flags() & ParameterFlags::Automatable;
            case Role::ShowAsList:
                return parameter->flags() & ParameterFlags::ShowAsList;
            case Role::ShowAsSwitch:
                return parameter->flags() & ParameterFlags::ShowAsSwitch;
            }
        }
    }
    return QVariant();
}

bool PluginParameterListModel::setData(
    const QModelIndex& index, const QVariant& value, int role)
{
    using namespace YADAW::Audio::Plugin;
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto parameter = pluginParameter_->parameter(row);
        if(parameter)
        {
            switch(role)
            {
            case Role::Value:
            {
                parameter->setValue(value.value<double>());
                dataChanged(index, index, {Role::Value});
                return true;
            }
            }
        }
    }
    return false;
}

QString PluginParameterListModel::stringFromValue(int index, double value) const
{
    auto parameter = pluginParameter_->parameter(index);
    if(parameter)
    {
        return parameter->valueToString(value);
    }
    return QString();
}

double PluginParameterListModel::valueFromString(int index, const QString& string) const
{
    auto parameter = pluginParameter_->parameter(index);
    if(parameter)
    {
        return parameter->stringToValue(string);
    }
    return NAN;
}
}