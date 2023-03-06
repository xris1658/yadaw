#include "PluginListModelImpl.hpp"

namespace YADAW::Model
{
PluginListModelImpl::PluginListModelImpl(const std::function<List()>& updateListFunc, QObject* parent):
    PluginListModel(parent),
    updateListFunc_(updateListFunc)
{
    update();
}

PluginListModelImpl::PluginListModelImpl(std::function<List()>&& updateListFunc, QObject* parent):
    PluginListModel(parent),
    updateListFunc_(std::move(updateListFunc))
{
    update();
}

PluginListModelImpl::~PluginListModelImpl()
{
}

int PluginListModelImpl::itemCount() const
{
    return data_.size();
}

int PluginListModelImpl::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginListModelImpl::data(const QModelIndex& index, int role) const
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
            return QVariant::fromValue(data.format);
        case Role::Type:
            return QVariant::fromValue(data.type);
        default:
            return {};
        }
    }
    return {};
}

void PluginListModelImpl::clear()
{
    beginRemoveRows(QModelIndex(), 0, data_.size() - 1);
    data_.clear();
    endRemoveRows();
}

void PluginListModelImpl::asyncUpdate()
{
    std::thread(std::mem_fn(&PluginListModelImpl::update), this).detach();
}

void PluginListModelImpl::update()
{
    data_ = updateListFunc_();
    beginInsertRows(QModelIndex(), 0, data_.size() - 1);
    endInsertRows();
}
}