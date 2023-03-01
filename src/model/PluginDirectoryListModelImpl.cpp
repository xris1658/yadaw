#include "PluginDirectoryListModelImpl.hpp"

namespace YADAW::Model
{
PluginDirectoryListModelImpl::PluginDirectoryListModelImpl(QObject* parent):
    PluginDirectoryListModel(parent)
{
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
        beginInsertRows(QModelIndex(), size, size);
        data_.emplace_back(path);
        endInsertRows();
    }
}

void PluginDirectoryListModelImpl::remove(int index)
{
    if(index < itemCount() && index >= 0)
    {
        beginRemoveRows(QModelIndex(), index, index);
        data_.erase(data_.begin() + index);
        endRemoveRows();
    }
}

void PluginDirectoryListModelImpl::clear()
{
    beginRemoveRows(QModelIndex(), 0, itemCount());
    data_.clear();
    endRemoveRows();
}
}