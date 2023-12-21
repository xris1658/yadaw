#include "SortFilterProxyListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
bool validateMapping(const std::vector<int>& srcToDst, const std::vector<int>& dstToSrc,
    std::vector<int>::iterator filteredOutFirst)
{
    if(srcToDst.size() == dstToSrc.size())
    {
        auto acceptedItemCount = filteredOutFirst - dstToSrc.begin();
        FOR_RANGE0(i, acceptedItemCount)
        {
            if(srcToDst[dstToSrc[i]] != i)
            {
                return false;
            }
        }
        FOR_RANGE(i, acceptedItemCount, dstToSrc.end() - dstToSrc.begin())
        {
            if(srcToDst[dstToSrc[i]] != -1)
            {
                return false;
            }
        }
        return true;
    }
    return true;
}

SortFilterProxyListModel::SortFilterProxyListModel(ISortFilterListModel& sourceModel, QObject* parent):
    ISortFilterProxyListModel(parent),
    sourceModel_(&sourceModel),
    filterRoleModel_(&sourceModel, this),
    sortOrderModel_(&sourceModel, this),
    srcToDst_(sourceModel.rowCount()),
    dstToSrc_(sourceModel.rowCount()),
    filteredOutFirst_(dstToSrc_.end()),
    filterString_()
{
    std::iota(srcToDst_.begin(), srcToDst_.end(), 0);
    std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsInserted,
        this, &SortFilterProxyListModel::sourceModelRowsInserted);
    QObject::connect(sourceModel_, &ISortFilterListModel::rowsAboutToBeRemoved,
        this, &SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved);
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

ISortOrderModel* SortFilterProxyListModel::getSortOrderModel()
{
    return &sortOrderModel_;
}

const ISortOrderModel* SortFilterProxyListModel::getSortOrderModel() const
{
    return &sortOrderModel_;
}

IFilterRoleModel* SortFilterProxyListModel::getFilterRoleModel()
{
    return &filterRoleModel_;
}

const IFilterRoleModel* SortFilterProxyListModel::getFilterRoleModel() const
{
    return &filterRoleModel_;
}

const QString& SortFilterProxyListModel::getFilterString() const
{
    return filterString_;
}

QString& SortFilterProxyListModel::getFilterString()
{
    return filterString_;
}

void SortFilterProxyListModel::setFilterString(const QString& filterString)
{
    filterString_ = filterString;
    doFilter();
    filterStringChanged();
}

bool SortFilterProxyListModel::insertSortOrder(int role, Qt::SortOrder sortOrder, int position)
{
    return sortOrderModel_.insert(role, sortOrder, position);
}

bool SortFilterProxyListModel::appendSortOrder(int role, Qt::SortOrder sortOrder)
{
    return sortOrderModel_.append(role, sortOrder);
}

int SortFilterProxyListModel::getSortIndexOfRole(int role) const
{
    return sortOrderModel_.getIndexOfRole(role);
}

bool SortFilterProxyListModel::removeSortOrder(int index)
{
    return sortOrderModel_.remove(index);
}

void SortFilterProxyListModel::clearSortOrder()
{
    sortOrderModel_.clear();
}

bool SortFilterProxyListModel::setFilter(int role, bool filterEnabled, Qt::CaseSensitivity caseSensitivity)
{
    return filterRoleModel_.setFilterRole(role, filterEnabled, caseSensitivity);
}

void SortFilterProxyListModel::clearFilter()
{
    return filterRoleModel_.clear();
}

int SortFilterProxyListModel::itemCount() const
{
    return filteredOutFirst_ - dstToSrc_.begin();
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

YADAW::Model::RoleNames SortFilterProxyListModel::roleNames() const
{
    return sourceModel_->roleNames();
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    const auto oldCount = itemCount();
    const auto newItemCount = last - first + 1;
    dstToSrc_.insert(filteredOutFirst_, newItemCount, -1);
    srcToDst_.insert(srcToDst_.begin() + first, newItemCount, -1);
    filteredOutFirst_ = dstToSrc_.begin() + oldCount;
    std::iota(filteredOutFirst_, filteredOutFirst_ + newItemCount, first);
    auto filteredOutFirst = std::stable_partition(filteredOutFirst_, filteredOutFirst_ + newItemCount,
        [this](int row)
        {
            return isAccepted(row, filterString_);
        }
    );
    mergeNewAcceptedItems(filteredOutFirst);
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    auto removeFirst = srcToDst_.begin() + first;
    auto removeLast = srcToDst_.begin() + last + 1;
    std::sort(removeFirst, removeLast);
    auto oldItemCount = itemCount();
    for(auto it = removeLast; it != removeFirst; --it)
    {
        auto dstRow = *it;
        if(dstRow != -1)
        {
            beginRemoveRows(QModelIndex(), dstRow, dstRow);
            dstToSrc_.erase(dstToSrc_.begin() + dstRow);
            endRemoveRows();
            --oldItemCount;
            --filteredOutFirst_;
        }
        else
        {
            dstToSrc_.erase(
                std::remove(
                    filteredOutFirst_, dstToSrc_.end(), it - srcToDst_.begin()
                )
            );
        }
    }
    srcToDst_.erase(removeFirst, removeLast);
}

void SortFilterProxyListModel::sourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{

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
    dataChanged(this->index(0), this->index(itemCount() - 1));
}

void SortFilterProxyListModel::sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    doSort();
    dataChanged(this->index(0), this->index(itemCount() - 1));
}

void SortFilterProxyListModel::sortOrderModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doSort();
    dataChanged(this->index(0), this->index(itemCount() - 1));
}

void SortFilterProxyListModel::filterRoleModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    auto outFirst = std::stable_partition(dstToSrc_.begin(), dstToSrc_.end(),
        [this, first, last](int row)
        {
            return isAccepted(row, filterString_, first, last + 1);
        }
    );
    std::for_each(outFirst, dstToSrc_.end(),
        [this](int row)
        {
            srcToDst_[row] = -1;
        }
    );
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::filterRoleModelRowsRemoved(
    const QModelIndex& parent, int first, int last)
{
    auto filteredOutFirst = std::stable_partition(filteredOutFirst_, dstToSrc_.end(),
        [this, first, last](int row)
        {
            return isAccepted(row, filterString_, first, last + 1);
        }
    );
    mergeNewAcceptedItems(filteredOutFirst);
}

void SortFilterProxyListModel::filterRoleModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    //
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
    return lhs < rhs;
}

bool SortFilterProxyListModel::isAccepted(int row, const QString& string) const
{
    return isAccepted(row, string, 0, filterRoleModel_.itemCount());
}

bool SortFilterProxyListModel::isAccepted(int row, const QString& string,
    int filterRoleBegin, int filterRoleEnd) const
{
    return filterRoleModel_.itemCount() == 0 || std::any_of(
        filterRoleModel_.cbegin() + filterRoleBegin, filterRoleModel_.cbegin() + filterRoleEnd,
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
    std::sort(dstToSrc_.begin(), dstToSrc_.end(),
        [this](int lhs, int rhs)
        {
            return isLess(lhs, rhs);
        }
    );
    FOR_RANGE0(i, dstToSrc_.size())
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::doFilter()
{
    auto oldFilteredOutFirst = filteredOutFirst_;
    auto outFirst = std::stable_partition(dstToSrc_.begin(), dstToSrc_.end(),
        [this](int row)
        {
            return isAccepted(row, filterString_);
        }
    );
    if(filteredOutFirst_ < outFirst)
    {
        beginInsertRows(QModelIndex(),
            oldFilteredOutFirst - dstToSrc_.begin(),
            outFirst - dstToSrc_.begin() - 1
        );
        filteredOutFirst_ = outFirst;
        endInsertRows();
        dataChanged(this->index(0), this->index(oldFilteredOutFirst - dstToSrc_.begin() - 1));
    }
    else
    {
        if(filteredOutFirst_ > outFirst)
        {
            beginRemoveRows(QModelIndex(),
                outFirst - dstToSrc_.begin(),
                oldFilteredOutFirst - dstToSrc_.begin() - 1
            );
            filteredOutFirst_ = outFirst;
            endRemoveRows();
        }
        if(outFirst != dstToSrc_.begin())
        {
            dataChanged(this->index(0), this->index(filteredOutFirst_ - dstToSrc_.begin() - 1));
        }
    }
    FOR_RANGE0(i, filteredOutFirst_ - dstToSrc_.begin())
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
    std::for_each(filteredOutFirst_, dstToSrc_.end(),
        [this](int row)
        {
            srcToDst_[row] = -1;
        }
    );
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::mergeNewAcceptedItems(
    std::vector<int>::iterator filteredOutFirst)
{
    auto newAcceptedItemCount = filteredOutFirst - filteredOutFirst_;
    std::sort(filteredOutFirst_, filteredOutFirst,
        [this](int lhs, int rhs)
        {
            return isLess(lhs, rhs);
        }
    );
    auto firstNewItem = filteredOutFirst_;
    auto beforeLast = firstNewItem + newAcceptedItemCount - 1;
    FOR_RANGE0(i, newAcceptedItemCount)
    {
        auto newFirstNewIterator = std::upper_bound(dstToSrc_.begin(), firstNewItem,
            *beforeLast,
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        const auto newFirstIndex = newFirstNewIterator - dstToSrc_.begin();
        beginInsertRows(QModelIndex(), newFirstIndex, newFirstIndex);
        srcToDst_[*beforeLast] = newFirstIndex + newAcceptedItemCount - 1 - i;
        std::rotate(newFirstNewIterator, beforeLast, beforeLast + 1);
        endInsertRows();
        ++firstNewItem;
    }
    filteredOutFirst_ = filteredOutFirst;
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}
}
