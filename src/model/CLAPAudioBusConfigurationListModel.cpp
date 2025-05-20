#include "CLAPAudioBusConfigurationListModel.hpp"

#include "entity/ChannelConfigHelper.hpp"

namespace YADAW::Model
{
CLAPAudioBusConfigurationListModel::CLAPAudioBusConfigurationListModel(
    YADAW::Audio::Plugin::CLAPPlugin& plugin, QObject* parent):
    ICLAPAudioBusConfigurationListModel(parent), plugin_(&plugin)
{}

CLAPAudioBusConfigurationListModel::~CLAPAudioBusConfigurationListModel()
{}

int CLAPAudioBusConfigurationListModel::rowCount(const QModelIndex& parent) const
{
    return plugin_->audioPortsConfigCount();
}

QVariant CLAPAudioBusConfigurationListModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < plugin_->audioPortsConfigCount())
    {
        auto& config = plugin_->audioPortsConfigAt(row)->get();
        switch(role)
        {
        case Role::Id:
            return QString::number(config.id());
        case Role::Name:
            return QString(config.name().toString());
        case Role::InputCount:
            return config.inputCount();
        case Role::OutputCount:
            return config.outputCount();
        case Role::HasMainInput:
            return config.hasMainInput();
        case Role::HasMainOutput:
            return config.hasMainOutput();
        case Role::MainInputChannelConfig:
        {
            if(config.hasMainInput())
            {
                return YADAW::Entity::configFromGroupType(config.mainInputType().first);
            }
            return {};
        }
        case Role::MainInputChannelCount:
        {
            if(config.hasMainInput())
            {
                return config.mainInputType().second;
            }
            return {};
        }
        case Role::MainOutputChannelConfig:
        {
            if(config.hasMainInput())
            {
                return YADAW::Entity::configFromGroupType(config.mainOutputType().first);
            }
            return {};
        }
        case Role::MainOutputChannelCount:
        {
            if(config.hasMainInput())
            {
                return config.mainOutputType().second;
            }
            return {};
        }
        }
    }
    return {};
}

bool CLAPAudioBusConfigurationListModel::isComparable(int roleIndex) const
{
    return false;
}

bool CLAPAudioBusConfigurationListModel::isFilterable(int roleIndex) const
{
    return false;
}

bool CLAPAudioBusConfigurationListModel::isSearchable(int roleIndex) const
{
    return false;
}

bool CLAPAudioBusConfigurationListModel::isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}

bool CLAPAudioBusConfigurationListModel::isSearchPassed(
    int roleIndex, const QModelIndex& modelIndex, const QString& string, Qt::CaseSensitivity) const
{
    return false;
}

bool CLAPAudioBusConfigurationListModel::isPassed(
    const QModelIndex& modelIndex, int role, const QVariant& variant) const
{
    return false;
}
}
