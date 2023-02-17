#include "PluginDirectoryListModel.hpp"

namespace YADAW::Model
{
PluginDirectoryListModel::PluginDirectoryListModel(QObject* parent):
    QAbstractListModel(parent)
{
}

PluginDirectoryListModel::~PluginDirectoryListModel()
{
}

int PluginDirectoryListModel::itemCount() const
{
    return data_.size();
}

constexpr int PluginDirectoryListModel::columnSize()
{
    return Role::RoleCount - Qt::UserRole;
}

int PluginDirectoryListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int PluginDirectoryListModel::columnCount(const QModelIndex&) const
{
    return columnSize();
}

QVariant PluginDirectoryListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row < itemCount() && row >= 0)
    {
        switch(role)
        {
        case Role::Path:
            return QVariant::fromValue(data_[row]);
        }
    }
    return {};
}

void PluginDirectoryListModel::append(const QString& path)
{
    if(std::find(data_.begin(), data_.end(), path) == data_.end())
    {
        auto size = itemCount();
        beginInsertRows(QModelIndex(), size, size);
        data_.emplace_back(path);
        endInsertRows();
    }
}

void PluginDirectoryListModel::remove(int index)
{
    if(index < itemCount() && index >= 0)
    {
        beginRemoveRows(QModelIndex(), index, index);
        data_.erase(data_.begin() + index);
        endRemoveRows();
    }
}

void PluginDirectoryListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, itemCount());
    data_.clear();
    endRemoveRows();
}

RoleNames PluginDirectoryListModel::roleNames() const
{
    return
    {
        std::make_pair(Role::Path, "path");
    };
}
}