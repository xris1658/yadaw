#include "SortFilterProxyListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
SortFilterProxyListModel::SortFilterProxyListModel(IComparableListModel& sourceModel, QObject* parent):
    ISortFilterProxyListModel(parent),
    sourceModel_(&sourceModel),
    sortOrderModel_(&sourceModel, this),
    srcToDst_(sourceModel.rowCount()),
    dstToSrc_(sourceModel.rowCount())
{
    std::iota(srcToDst_.begin(), srcToDst_.end(), 0);
    std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
    QObject::connect(sourceModel_, &IComparableListModel::rowsAboutToBeInserted,
        this, &SortFilterProxyListModel::sourceModelRowsAboutToBeInserted);
    QObject::connect(sourceModel_, &IComparableListModel::rowsInserted,
        this, &SortFilterProxyListModel::sourceModelRowsInserted);
    QObject::connect(sourceModel_, &IComparableListModel::rowsAboutToBeRemoved,
        this, &SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved);
    QObject::connect(sourceModel_, &IComparableListModel::rowsRemoved,
        this, &SortFilterProxyListModel::sourceModelRowsRemoved);
    QObject::connect(sourceModel_, &IComparableListModel::dataChanged,
        this, &SortFilterProxyListModel::sourceModelDataChanged);
    QObject::connect(sourceModel_, &IComparableListModel::modelAboutToBeReset,
        this, &SortFilterProxyListModel::sourceModelAboutToBeReset);
    QObject::connect(sourceModel_, &IComparableListModel::modelReset,
        this, &SortFilterProxyListModel::sourceModelReset);
    QObject::connect(&sortOrderModel_, &SortOrderModel::rowsInserted,
        this, &SortFilterProxyListModel::sortOrderModelRowsInserted);
    QObject::connect(&sortOrderModel_, &SortOrderModel::rowsRemoved,
        this, &SortFilterProxyListModel::sortOrderModelRowsRemoved);
    QObject::connect(&sortOrderModel_, &SortOrderModel::dataChanged,
        this, &SortFilterProxyListModel::sortOrderModelDataChanged);
}

SortFilterProxyListModel::~SortFilterProxyListModel()
{}

IComparableListModel* SortFilterProxyListModel::sourceModel()
{
    return sourceModel_;
}

const IComparableListModel* SortFilterProxyListModel::sourceModel() const
{
    return sourceModel_;
}

ISortOrderModel* SortFilterProxyListModel::sortOrderModel()
{
    return &sortOrderModel_;
}

const ISortOrderModel* SortFilterProxyListModel::sortOrderModel() const
{
    return &sortOrderModel_;
}

int SortFilterProxyListModel::itemCount() const
{
    return dstToSrc_.size();
}

int SortFilterProxyListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int SortFilterProxyListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    const auto oldCount = itemCount();
    const auto newItemCount = last - first + 1;
    srcToDst_.insert(srcToDst_.begin() + first, newItemCount, oldCount);
    std::iota(srcToDst_.begin() + first + 1, srcToDst_.begin() + first + newItemCount, oldCount + 1);
    dstToSrc_.insert(dstToSrc_.end(), newItemCount, first);
    std::iota(dstToSrc_.begin() + oldCount + 1, dstToSrc_.end(), first + 1);
    std::stable_sort(dstToSrc_.begin(), dstToSrc_.end(),
        [this](int lhs, int rhs) { return isLess(lhs, rhs); });
    FOR_RANGE0(i, dstToSrc_.size())
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
    auto min = std::min_element(srcToDst_.begin() + first, srcToDst_.begin() + last - 1);
    beginInsertRows({}, oldCount, srcToDst_.size());
    endInsertRows();
    emit dataChanged(this->index(*min), this->index(srcToDst_.size()));
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{}

void SortFilterProxyListModel::sourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    if(last - first + 1 == srcToDst_.size())
    {
        beginRemoveRows({}, first, last);
        endRemoveRows();
        return;
    }
    auto itFirst = srcToDst_.begin() + first;
    auto itLast = srcToDst_.begin() + last + 1;
    auto remove = std::remove_if(dstToSrc_.begin(), dstToSrc_.end(),
        [itFirst, itLast](int index)
        {
            return std::find(itFirst, itLast, index) != itLast;
        }
    );
    std::sort(itFirst, itLast, std::greater<int>());
    std::for_each(itFirst, itLast,
        [this](int index)
        {
            beginRemoveRows({}, index, index);
            endRemoveRows();
        }
    );
}

void SortFilterProxyListModel::sourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    //
}

void SortFilterProxyListModel::sourceModelAboutToBeReset()
{

}

void SortFilterProxyListModel::sourceModelReset()
{

}

void SortFilterProxyListModel::sortOrderModelRowsInserted(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sortOrderModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{

}

bool SortFilterProxyListModel::isLess(int lhsRow, int rhsRow) const
{
    auto lhs = sourceModel_->index(lhsRow);
    auto rhs = sourceModel_->index(rhsRow);
    if(lhs.model() == this && rhs.model() == this)
    {
        FOR_RANGE0(i, sortOrderModel_.itemCount())
        {
            const auto& [role, order] = sortOrderModel_[i];
            if(order == Qt::AscendingOrder)
            {
                if(sourceModel_->isLess(role, lhs, rhs))
                {
                    return true;
                }
                if(sourceModel_->isLess(role, rhs, lhs))
                {
                    return false;
                }
            }
            else
            {
                if(sourceModel_->isLess(role, lhs, rhs))
                {
                    return false;
                }
                if(sourceModel_->isLess(role, rhs, lhs))
                {
                    return true;
                }
            }
        }
    }
    return false;
}
}
