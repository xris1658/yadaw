#include "model/SortOrderModel.hpp"

namespace YADAW::Model
{
SortOrderModel::SortOrderModel(QObject* parent):
    QAbstractListModel(parent)
{}

SortOrderModel::SortOrderModel(const SortOrderModel& rhs):
    QAbstractListModel(rhs.parent()),
    sortOrder_(rhs.sortOrder_)
{}

SortOrderModel::~SortOrderModel()
{}

int SortOrderModel::itemCount() const
{
    return sortOrder_.size();
}

OptionalRef<const std::pair<int, Qt::SortOrder>> SortOrderModel::at(std::size_t index) const
{
    if(index < itemCount())
    {
        return {operator[](index)};
    }
    return std::nullopt;
}

OptionalRef<std::pair<int, Qt::SortOrder>> SortOrderModel::at(std::size_t index)
{
    if(index < itemCount())
    {
        return {operator[](index)};
    }
    return std::nullopt;
}

const std::pair<int, Qt::SortOrder>& SortOrderModel::operator[](std::size_t index) const
{
    return sortOrder_[index];
}

std::pair<int, Qt::SortOrder>& SortOrderModel::operator[](std::size_t index)
{
    return sortOrder_[index];
}

int SortOrderModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int SortOrderModel::columnCount(const QModelIndex&) const
{
    return columnCount();
}

QVariant SortOrderModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& [sortRole, order] = sortOrder_[row];
        switch(role)
        {
        case Role::SortRole:
            return QVariant::fromValue(sortRole);
        case Role::SortOrder:
            return QVariant::fromValue<int>(order);
        }
    }
    return {};
}

bool SortOrderModel::insert(int role, Qt::SortOrder sortOrder, int position)
{
    auto it = std::find_if(sortOrder_.begin(), sortOrder_.end(),
        [role](decltype(sortOrder_)::const_reference pair)
        {
            const auto& [sortRole, order] = pair;
            return sortRole == role;
        }
    );
    if(it == sortOrder_.end() && position >= 0 && position <= itemCount())
    {
        beginInsertRows(QModelIndex(), position, position);
        sortOrder_.emplace(sortOrder_.begin() + position, role, sortOrder);
        endInsertRows();
        return true;
    }
    return false;
}

bool SortOrderModel::append(int role, Qt::SortOrder sortOrder)
{
    return insert(role, sortOrder, itemCount());
}

int SortOrderModel::getIndexOfRole(int role) const
{
    auto it = std::find_if(sortOrder_.begin(), sortOrder_.end(),
        [role](decltype(sortOrder_)::const_reference pair)
        {
            const auto& [sortRole, order] = pair;
            return sortRole == role;
        }
    );
    return it == sortOrder_.end()? -1: it - sortOrder_.begin();
}

bool SortOrderModel::remove(int index)
{
    if(index >= 0 && index < itemCount())
    {
        beginRemoveRows(QModelIndex(), index, index);
        sortOrder_.erase(sortOrder_.begin() + index);
        endRemoveRows();
        return true;
    }
    return false;
}

void SortOrderModel::clear()
{
    if(auto count = itemCount(); count != 0)
    {
        beginRemoveRows(QModelIndex(), 0, count - 1);
        sortOrder_.clear();
        endRemoveRows();
    }
}

RoleNames SortOrderModel::roleNames() const
{
    static RoleNames ret
    {
        std::make_pair(Role::SortRole, "som_sort_role"),
        std::make_pair(Role::SortOrder, "som_sort_order")
    };
    return ret;
}
}