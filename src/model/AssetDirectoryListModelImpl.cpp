#include "AssetDirectoryListModelImpl.hpp"

namespace YADAW::Model
{
AssetDirectoryListModelImpl::AssetDirectoryListModelImpl(QObject* parent):
    AssetDirectoryListModel(parent)
{
}

AssetDirectoryListModelImpl::~AssetDirectoryListModelImpl()
{
}

int AssetDirectoryListModelImpl::itemCount() const
{
    return data_.size();
}

int AssetDirectoryListModelImpl::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AssetDirectoryListModelImpl::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < itemCount())
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
        default:
            return {};
        }
    }
    return {};
}

bool AssetDirectoryListModelImpl::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& [id, path, name] = data_[row];
        switch(role)
        {
        case Role::Id:
            return false;
        case Role::Path:
        {
            if(value.canConvert<QString>())
            {
                path = value.value<QString>();
                return true;
            }
            return false;
        }
        case Role::Name:
        {
            if(value.canConvert<QString>())
            {
                name = value.value<QString>();
                return true;
            }
            return false;
        }
        default:
            return false;
        }
    }
    return false;
}

void AssetDirectoryListModelImpl::append(int id, const QString& path, const QString& name)
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

void AssetDirectoryListModelImpl::remove(int id)
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

void AssetDirectoryListModelImpl::clear()
{
    beginRemoveRows(QModelIndex(), 0, itemCount());
    data_.clear();
    endRemoveRows();
}
}