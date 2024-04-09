#include "AssetDirectoryListModel.hpp"

#include "dao/AssetDirectoryTable.hpp"

#include <QDir>
#include <QFileInfo>

namespace YADAW::Model
{
AssetDirectoryListModel::AssetDirectoryListModel(QObject* parent):
    IAssetDirectoryListModel(parent)
{
    try
    {
        const auto& list = YADAW::DAO::selectAllAssetDirectories();
        for(const auto& [id, path, name]: list)
        {
            beginResetModel();
            data_.emplace_back(
                path, name, std::make_unique<FileTreeModel>(path), id);
            endResetModel();
        }
    }
    catch(...) {}
}

AssetDirectoryListModel::~AssetDirectoryListModel()
{
}

std::uint32_t AssetDirectoryListModel::itemCount() const
{
    return data_.size();
}

int AssetDirectoryListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AssetDirectoryListModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& [path, name, fileTreeModel, id] = data_[row];
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(id);
        case Role::Path:
            return QVariant::fromValue(path);
        case Role::Name:
            return QVariant::fromValue(name);
        case Role::DefaultName:
            return QVariant::fromValue(QFileInfo(path).baseName());
        case Role::FileTree:
            return QVariant::fromValue<QObject*>(fileTreeModel.get());
        default:
            return {};
        }
    }
    return {};
}

bool AssetDirectoryListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& [path, name, fileTreeModel, id] = data_[row];
        switch(role)
        {
        case Role::Id:
            return false;
        case Role::Path:
        {
            if(value.canConvert<QString>())
            {
                path = value.value<QString>();
                fileTreeModel = std::make_unique<FileTreeModel>(path);
                dataChanged(index, index, {Role::Path, Role::FileTree});
                return true;
            }
            return false;
        }
        case Role::Name:
        {
            if(value.canConvert<QString>())
            {
                name = value.value<QString>();
                dataChanged(index, index, {Role::Name});
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

void AssetDirectoryListModel::append(const QString& path, const QString& name)
{
    if(std::find_if(data_.begin(), data_.end(),
        [&path](const Data& data)
        {
            const auto& [dataPath, dataName, fileTreeModel, id] = data;
            return dataPath == path;
        }) == data_.end())
    {
        auto size = itemCount();
        try
        {
            auto id = YADAW::DAO::addAssetDirectory(path, name);
            beginInsertRows(QModelIndex(), size, size);
            data_.emplace_back(
                path, name,
                std::make_unique<YADAW::Model::FileTreeModel>(path),
                id
            );
            endInsertRows();
        }
        catch(...) {}
    }
}

void AssetDirectoryListModel::append(const QUrl& url)
{
    if(url.isLocalFile())
    {
        const auto& path = url.toLocalFile();
        QDir dir(path);
        if(dir.exists())
        {
            append(QDir::toNativeSeparators(dir.absolutePath()), dir.dirName());
        }
    }
}

void AssetDirectoryListModel::rename(int id, const QString& name)
{
    if(auto it = std::find_if(data_.begin(), data_.end(),
        [toFind = id](const Data& data)
        {
            const auto& [dataPath, dataName, fileTreeModel, id] = data;
            return toFind == id;
        }); it != data_.end())
    {
        auto row = std::distance(data_.begin(), it);
        auto& [dataPath, dataName, fileTreeModel, id] = *it;
        try
        {
            YADAW::DAO::updateAssetDirectoryName(id, name);
        }
        catch(...) {}
        dataName = name;
        dataChanged(index(row), index(row), {Role::Name});
    }
}

void AssetDirectoryListModel::remove(int id)
{
    if(auto it = std::find_if(data_.begin(), data_.end(),
        [toFind = id](const Data& data)
        {
            const auto& [dataPath, dataName, fileTreeModel, id] = data;
            return id == toFind;
        }); it != data_.end())
    {
        try
        {
            YADAW::DAO::removeAssetDirectory(id);
            auto row = std::distance(data_.begin(), it);
            beginRemoveRows(QModelIndex(), row, row);
            data_.erase(it);
            endRemoveRows();
        }
        catch(...) {}
    }
}
}