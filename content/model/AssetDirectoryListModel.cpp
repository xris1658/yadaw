#include "AssetDirectoryListModel.hpp"

namespace YADAW::Model
{
AssetDirectoryListModel::AssetDirectoryListModel(QObject* parent):
    QAbstractListModel(parent)
{
}

AssetDirectoryListModel::~AssetDirectoryListModel()
{
}

int AssetDirectoryListModel::itemCount() const
{
    return data_.size();
}

constexpr int AssetDirectoryListModel::columnSize()
{
    return Role::RoleCount - Qt::UserRole;
}

int AssetDirectoryListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int AssetDirectoryListModel::columnCount(const QModelIndex&) const
{
    return columnSize();
}

QVariant AssetDirectoryListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row < itemCount() && row >= 0)
    {
        const auto& [id, path, name] = data_[row];
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(id);
        case Role::Path:
            return QVariant::fromValue(path);
        case Role::Name:
            return QVariant::fromValue(name);
        }
    }
    return {};
}

void AssetDirectoryListModel::append(int id, const QString& path, const QString& name)
{
    if(std::find_if(data_.begin(), data_.end(),
        [toFind = id](const auto& tuple)
    {
        const auto& [dataId, dataPath, dataName] = tuple;
        return dataId == toFind;
    }) == data_.end())
    {
        auto size = itemCount();
        beginInsertRows(QModelIndex(), size, size);
        data_.emplace_back(id, path, name);
    }
}

void AssetDirectoryListModel::remove(int id)
{
    if(auto it = std::find_if(data_.begin(), data_.end(),
        [toFind = id](const auto& tuple)
        {
            const auto& [dataId, dataPath, dataName] = tuple;
            return dataId == toFind;
        }); it != data_.end())
    {
        auto index = std::distance(data_.begin(), it);
        beginRemoveRows(QModelIndex(), index, index);
        data_.erase(it);
        endRemoveRows();
    }
}

void AssetDirectoryListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, itemCount());
    data_.clear();
    endRemoveRows();
}

RoleNames AssetDirectoryListModel::roleNames() const
{
    return
    {
        std::make_pair(Role::Id, "pathId"),
        std::make_pair(Role::Path, "path"),
        std::make_pair(Role::Name, "name")
    };
}
}
