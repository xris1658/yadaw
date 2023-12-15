#include "SortFilterProxyListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
SortFilterProxyListModel::SortFilterProxyListModel(ISortFilterListModel& sourceModel, QObject* parent):
    ISortFilterProxyListModel(parent),
    sourceModel_(&sourceModel),
    filterRoleModel_(&sourceModel, this),
    sortOrderModel_(&sourceModel, this),
    srcToDst_(sourceModel.rowCount()),
    dstToSrc_(sourceModel.rowCount())
{
    std::iota(srcToDst_.begin(), srcToDst_.end(), 0);
    std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsAboutToBeInserted,
        this, &SortFilterProxyListModel::sourceModelRowsAboutToBeInserted);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsInserted,
        this, &SortFilterProxyListModel::sourceModelRowsInserted);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsAboutToBeRemoved,
        this, &SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsRemoved,
        this, &SortFilterProxyListModel::sourceModelRowsRemoved);
    QObject::connect(sourceModel_, &ISortFilterListModel::dataChanged,
        this, &SortFilterProxyListModel::sourceModelDataChanged);
    QObject::connect(sourceModel_, &ISortFilterListModel::modelAboutToBeReset,
        this, &SortFilterProxyListModel::sourceModelAboutToBeReset);
    QObject::connect(sourceModel_, &ISortFilterListModel::modelReset,
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

ISortFilterListModel* SortFilterProxyListModel::sourceModel()
{
    return sourceModel_;
}

const ISortFilterListModel* SortFilterProxyListModel::sourceModel() const
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

IFilterRoleModel* SortFilterProxyListModel::filterRoleModel()
{
    return &filterRoleModel_;
}

const IFilterRoleModel* SortFilterProxyListModel::filterRoleModel() const
{
    return &filterRoleModel_;
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

QVariant SortFilterProxyListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        return sourceModel_->data(sourceModel_->index(dstToSrc_[row]), role);
    }
    return {};
}

bool SortFilterProxyListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        return sourceModel_->setData(sourceModel_->index(dstToSrc_[row]), value, role);
    }
    return false;
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
    beginInsertRows({}, first, last);
}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    const auto oldCount = itemCount();
    const auto newItemCount = last - first + 1;
    srcToDst_.insert(srcToDst_.begin() + first, newItemCount, oldCount);
    std::iota(srcToDst_.begin() + first + 1, srcToDst_.begin() + first + newItemCount, oldCount + 1);
    dstToSrc_.insert(dstToSrc_.end(), newItemCount, first);
    std::iota(dstToSrc_.begin() + oldCount + 1, dstToSrc_.end(), first + 1);
    doSort();
    auto min = std::min_element(srcToDst_.begin() + first, srcToDst_.begin() + last - 1);
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
    std::for_each(remove, itLast,
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
    std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
    doSort();
    dataChanged(this->index(0), this->index(itemCount()));
}

void SortFilterProxyListModel::sourceModelAboutToBeReset()
{
    beginResetModel();
}

void SortFilterProxyListModel::sourceModelReset()
{
    auto itemCount = sourceModel_->rowCount();
    srcToDst_.resize(itemCount, 0);
    dstToSrc_.resize(itemCount, 0);
    std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
    doSort();
    endResetModel();
}

void SortFilterProxyListModel::sortOrderModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    doSort();
    dataChanged(this->index(0), this->index(itemCount()));
}

void SortFilterProxyListModel::sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    doSort();
    dataChanged(this->index(0), this->index(itemCount()));
}

void SortFilterProxyListModel::sortOrderModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doSort();
    dataChanged(this->index(0), this->index(itemCount()));
}

bool SortFilterProxyListModel::isLess(int lhsRow, int rhsRow) const
{
    auto lhs = sourceModel_->index(lhsRow);
    auto rhs = sourceModel_->index(rhsRow);
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

bool SortFilterProxyListModel::isPassed(int row, const QString& string) const
{
    return std::all_of(filterRoleModel_.cbegin(), filterRoleModel_.cend(),
        [sourceModel = this->sourceModel_, &string, index = sourceModel_->index(row)]
        (const std::pair<int, Qt::CaseSensitivity>& pair)
        {
            const auto& [role, caseSensitivity] = pair;
            return sourceModel->isPassed(role, index, string, caseSensitivity);
        }
    );
}

void SortFilterProxyListModel::doSort()
{
    std::stable_sort(dstToSrc_.begin(), dstToSrc_.end(),
        [this](int lhs, int rhs)
        {
            return isLess(lhs, rhs);
        }
    );
    FOR_RANGE0(i, dstToSrc_.size())
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
}

void SortFilterProxyListModel::doFilter(const QString& string)
{
    auto remove = std::remove_if(dstToSrc_.begin(), dstToSrc_.end(),
        [this, &string](int row)
        {
            return isPassed(row, string);
        }
    );
    std::for_each(remove, dstToSrc_.end(),
        [this](int row)
        {
            srcToDst_[row] = -1;
        }
    );
}
}
