#include "PluginDirectoryListModelImpl.hpp"

#include "dao/PluginDirectoryTable.hpp"

#include <QDir>

namespace YADAW::Model
{
PluginDirectoryListModelImpl::PluginDirectoryListModelImpl(QObject* parent):
    PluginDirectoryListModel(parent)
{
    try
    {
        data_ = YADAW::DAO::selectPluginDirectory();
    }
    catch(...) {}
}

PluginDirectoryListModelImpl::~PluginDirectoryListModelImpl()
{
}

int PluginDirectoryListModelImpl::itemCount() const
{
    return data_.size();
}

int PluginDirectoryListModelImpl::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginDirectoryListModelImpl::data(const QModelIndex& index, int role) const
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

bool PluginDirectoryListModelImpl::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row < itemCount() && row >= 0)
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

void PluginDirectoryListModelImpl::append(const QString& path)
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

void PluginDirectoryListModelImpl::append(const QUrl& url)
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

void PluginDirectoryListModelImpl::remove(int index)
{
    if(index < itemCount() && index >= 0)
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