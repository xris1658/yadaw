#include "PluginCategoryListModel.hpp"

#include <thread>

namespace YADAW::Model
{
PluginCategoryListModel::PluginCategoryListModel(
    const std::function<List()>& updateListFunc, QObject* parent):
    IPluginCategoryListModel(parent),
    updateListFunc_(updateListFunc)
{
    update();
}

PluginCategoryListModel::PluginCategoryListModel(
    std::function<List()>&& updateListFunc, QObject* parent):
        IPluginCategoryListModel(parent),
        updateListFunc_(std::move(updateListFunc))
{
    update();
}

PluginCategoryListModel::~PluginCategoryListModel()
{}

int PluginCategoryListModel::itemCount() const
{
    return data_.size();
}

int PluginCategoryListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginCategoryListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& data = data_[row];
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(data.id);
        case Role::Name:
            return QVariant::fromValue(data.category);
        default:
            return {};
        }
    }
    return {};
}

void PluginCategoryListModel::update()
{
    const auto& list = updateListFunc_();
    if(!data_.empty())
    {
        beginRemoveRows(QModelIndex(), 0, data_.size() - 1);
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, list.size() - 1);
    data_ = std::move(list);
    endInsertRows();
}
}