#include "PluginListModel.hpp"

#include <thread>

namespace YADAW::Model
{
namespace Impl
{
YADAW::Model::IPluginListModel::PluginFormat getPluginFormat(int format)
{
    switch(format)
    {
    case YADAW::DAO::PluginFormat::PluginFormatVST3:
        return YADAW::Model::IPluginListModel::PluginFormat::VST3;
    case YADAW::DAO::PluginFormat::PluginFormatCLAP:
        return YADAW::Model::IPluginListModel::PluginFormat::CLAP;
    default:
        return YADAW::Model::IPluginListModel::PluginFormat::UnknownFormat;
    }
}

YADAW::Model::IPluginListModel::PluginType getPluginType(int type)
{
    switch(type)
    {
    case YADAW::DAO::PluginType::PluginTypeMIDIEffect:
        return YADAW::Model::IPluginListModel::PluginType::MIDIEffect;
    case YADAW::DAO::PluginType::PluginTypeInstrument:
        return YADAW::Model::IPluginListModel::PluginType::Instrument;
    case YADAW::DAO::PluginType::PluginTypeAudioEffect:
        return YADAW::Model::IPluginListModel::PluginType::AudioEffect;
    default:
        return YADAW::Model::IPluginListModel::PluginType::UnknownType;
    }
}
}

PluginListModel::PluginListModel(const std::function<List()>& updateListFunc, QObject* parent):
    IPluginListModel(parent),
    updateListFunc_(updateListFunc)
{
    update();
}

PluginListModel::PluginListModel(std::function<List()>&& updateListFunc, QObject* parent):
    IPluginListModel(parent),
    updateListFunc_(std::move(updateListFunc))
{
    update();
}

PluginListModel::~PluginListModel()
{
}

int PluginListModel::itemCount() const
{
    return data_.size();
}

int PluginListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& data = data_[row];
        // Note that Role::Uid is not included since we don't need to "show" or "use" the plugin UID
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(data.id);
        case Role::Path:
            return QVariant::fromValue(data.path);
        case Role::Name:
            return QVariant::fromValue(data.name);
        case Role::Vendor:
            return QVariant::fromValue(data.vendor);
        case Role::Version:
            return QVariant::fromValue(data.version);
        case Role::Format:
            return QVariant::fromValue<int>(Impl::getPluginFormat(data.format));
        case Role::Type:
            return QVariant::fromValue<int>(Impl::getPluginType(data.type));
        default:
            return {};
        }
    }
    return {};
}

void PluginListModel::clear()
{
    if(!data_.empty())
    {
        beginRemoveRows(QModelIndex(), 0, data_.size() - 1);
        data_.clear();
        endRemoveRows();
    }
}

void PluginListModel::asyncUpdate()
{
    std::thread(std::mem_fn(&PluginListModel::update), this).detach();
}

void PluginListModel::update()
{
    const auto& list = updateListFunc_();
    if(!list.empty())
    {
        beginInsertRows(QModelIndex(), 0, list.size() - 1);
        data_ = std::move(list);
        endInsertRows();
    }
}
}