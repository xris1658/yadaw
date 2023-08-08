#include "PluginDirectoryListModel.hpp"

#include "dao/PluginDirectoryTable.hpp"

#include <QDir>

namespace YADAW::Model
{
PluginDirectoryListModel::PluginDirectoryListModel(QObject* parent):
    IPluginDirectoryListModel(parent)
{
    try
    {
        data_ = YADAW::DAO::selectPluginDirectory();
    }
    catch(...) {}
}

PluginDirectoryListModel::~PluginDirectoryListModel()
{
}

int PluginDirectoryListModel::itemCount() const
{
    return data_.size();
}

const QString& PluginDirectoryListModel::at(int i) const
{
    return data_.at(i);
}

const QString& PluginDirectoryListModel::operator[](int i) const
{
    return data_[i];
}

int PluginDirectoryListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginDirectoryListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Path:
            return QVariant::fromValue(data_[row]);
        }
    }
    return {};
}

bool PluginDirectoryListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Path:
        {
            if(value.canConvert<QString>())
            {
                data_[row] = value.value<QString>();
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

void PluginDirectoryListModel::append(const QString& path)
{
    if(std::find(data_.begin(), data_.end(), path) == data_.end())
    {
        auto size = itemCount();
        try
        {
            YADAW::DAO::addPluginDirectory(path);
            beginInsertRows(QModelIndex(), size, size);
            data_.emplace_back(path);
            endInsertRows();
        }
        catch(...) {}
    }
}

void PluginDirectoryListModel::append(const QUrl& url)
{
    if(url.isLocalFile())
    {
        const auto& path = url.toLocalFile();
        QDir dir(path);
        if(dir.exists())
        {
            append(QDir::toNativeSeparators(dir.absolutePath()));
        }
    }
}

void PluginDirectoryListModel::remove(int index)
{
    if(index >= 0 && index < itemCount())
    {
        try
        {
            YADAW::DAO::removePluginDirectory(data_[index]);
            beginRemoveRows(QModelIndex(), index, index);
            data_.erase(data_.begin() + index);
            endRemoveRows();
        }
        catch(...) {}
    }
}

}