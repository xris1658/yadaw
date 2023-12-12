#include "SortOrderModel.hpp"

namespace YADAW::Model
{
SortOrderModel::SortOrderModel(IComparableListModel* model, QObject* parent):
    ISortOrderModel(parent),
    model_(model)
{}

SortOrderModel::~SortOrderModel()
{}

int SortOrderModel::itemCount() const
{
    return sortOrder_.size();
}

int SortOrderModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant SortOrderModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& [role, order] = sortOrder_[row];
        switch(role)
        {
        case Role::SortRole:
            return QVariant::fromValue(role);
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
    if(it == sortOrder_.end() && model_->isComparable(role)
        && position >= 0 && position <= itemCount())
    {
        beginInsertRows(QModelIndex(), position, position);
        sortOrder_.emplace(sortOrder_.begin() + position, role, sortOrder);
        endInsertRows();
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
    return it - sortOrder_.begin();
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
}