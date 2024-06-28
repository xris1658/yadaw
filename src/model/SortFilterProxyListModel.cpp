#include "model/SortFilterProxyListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
void validateMapping(const std::vector<int>& srcToDst, const std::vector<int>& dstToSrc)
{
    assert(srcToDst.size() == dstToSrc.size());
    FOR_RANGE0(i, srcToDst.size())
    {
        assert(srcToDst[dstToSrc[i]] == i);
    }
}

SortFilterProxyListModel::SortFilterProxyListModel(QObject* parent):
    QAbstractListModel(parent)
{
    acceptedItemCount_ = 0;
    QObject::connect(&sortOrderModel_, &SortOrderModel::rowsInserted,
        this, &SortFilterProxyListModel::sortOrderModelRowsInserted);
    QObject::connect(&sortOrderModel_, &SortOrderModel::rowsRemoved,
        this, &SortFilterProxyListModel::sortOrderModelRowsRemoved);
    QObject::connect(&sortOrderModel_, &SortOrderModel::dataChanged,
        this, &SortFilterProxyListModel::sortOrderModelDataChanged);
    QObject::connect(&filterRoleModel_, &FilterRoleModel::rowsInserted,
        this, &SortFilterProxyListModel::filterRoleModelRowsInserted);
    QObject::connect(&filterRoleModel_, &FilterRoleModel::rowsRemoved,
        this, &SortFilterProxyListModel::filterRoleModelRowsRemoved);
    QObject::connect(&filterRoleModel_, &FilterRoleModel::dataChanged,
        this, &SortFilterProxyListModel::filterRoleModelDataChanged);
}

SortFilterProxyListModel::SortFilterProxyListModel(
    ISortFilterListModel& sourceModel, QObject* parent):
    SortFilterProxyListModel(parent)
{
    setSourceModel(&sourceModel);
}

SortFilterProxyListModel::SortFilterProxyListModel(
    const SortFilterProxyListModel& rhs):
    SortFilterProxyListModel(rhs.parent())
{
    setSourceModel(rhs.sourceModel_);
}

SortFilterProxyListModel::~SortFilterProxyListModel()
{}

ISortFilterListModel* SortFilterProxyListModel::getSourceModel()
{
    return sourceModel_;
}

const ISortFilterListModel* SortFilterProxyListModel::getSourceModel() const
{
    return sourceModel_;
}

void SortFilterProxyListModel::setSourceModel(ISortFilterListModel* model)
{
    if(model != sourceModel_)
    {
        auto oldItemCount = itemCount();
        if(oldItemCount != 0)
        {
            beginRemoveRows(QModelIndex(), 0, oldItemCount - 1);
        }
        if(sourceModel_)
        {
            for(const auto& connection: connections_)
            {
                QObject::disconnect(connection);
            }
            srcToDst_.clear();
            srcToDst_.shrink_to_fit();
            dstToSrc_.clear();
            dstToSrc_.shrink_to_fit();
            acceptedItemCount_ = 0;
        }
        sourceModel_ = model;
        if(oldItemCount != 0)
        {
            endRemoveRows();
        }
        if(model)
        {
            connections_[0] = QObject::connect(
                sourceModel_, &ISortFilterListModel::rowsInserted,
                this, &SortFilterProxyListModel::sourceModelRowsInserted);
            connections_[1] = QObject::connect(
                sourceModel_, &ISortFilterListModel::rowsAboutToBeRemoved,
                this, &SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved);
            connections_[2] = QObject::connect(
                sourceModel_, &ISortFilterListModel::rowsRemoved,
                this, &SortFilterProxyListModel::sourceModelRowsRemoved);
            connections_[3] = QObject::connect(
                sourceModel_, &ISortFilterListModel::dataChanged,
                this, &SortFilterProxyListModel::sourceModelDataChanged);
            connections_[4] = QObject::connect(
                sourceModel_, &ISortFilterListModel::modelAboutToBeReset,
                this, &SortFilterProxyListModel::sourceModelAboutToBeReset);
            connections_[5] = QObject::connect(
                sourceModel_, &ISortFilterListModel::modelReset,
                this, &SortFilterProxyListModel::sourceModelReset);
            YADAW::Util::IntegerRange<int> insertRange(model->rowCount());
            srcToDst_.reserve(*insertRange.end());
            dstToSrc_.reserve(*insertRange.end());
            std::copy(insertRange.begin(), insertRange.end(), std::back_inserter(srcToDst_));
            std::copy(insertRange.begin(), insertRange.end(), std::back_inserter(dstToSrc_));
            auto newItemCount = itemCount();
            if(newItemCount != 0)
            {
                beginInsertRows(QModelIndex(), 0, itemCount() - 1);
            }
            auto partitionPoint = std::partition(
                dstToSrc_.begin(), dstToSrc_.end(),
                [this](int srcRow) { return isAccepted(srcRow); }
            );
            updateSrcToDst(0, dstToSrc_.size());
            mergeNewAcceptedItems(
                partitionPoint - dstToSrc_.begin()
            );
        }
        sourceModelChanged();
    }
}

SortOrderModel* SortFilterProxyListModel::getSortOrderModel()
{
    return &sortOrderModel_;
}

const SortOrderModel* SortFilterProxyListModel::getSortOrderModel() const
{
    return &sortOrderModel_;
}

FilterRoleModel* SortFilterProxyListModel::getFilterRoleModel()
{
    return &filterRoleModel_;
}

const FilterRoleModel* SortFilterProxyListModel::getFilterRoleModel() const
{
    return &filterRoleModel_;
}

QString SortFilterProxyListModel::getFilterString() const
{
    return filterString_;
}

void SortFilterProxyListModel::setFilterString(const QString& filterString)
{
    filterString_ = filterString;
    doFilter();
    filterStringChanged();
}

int SortFilterProxyListModel::mapToSource(int destIndex) const
{
    if(destIndex >= 0 && destIndex < itemCount())
    {
        return dstToSrc_[destIndex];
    }
    return -1;
}

int SortFilterProxyListModel::mapFromSource(int sourceIndex) const
{
    if(sourceModel_ && sourceIndex >= 0 && sourceIndex < sourceModel_->rowCount())
    {
        return srcToDst_[sourceIndex];
    }
    return -1;
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

QVariant SortFilterProxyListModel::valueOfFilter(int role) const
{
    auto it = valuesOfFilter_.find(role);
    return it != valuesOfFilter_.end()? it->second: QVariant();
}

bool SortFilterProxyListModel::setValueOfFilter(int role, const QVariant& value)
{
    valuesOfFilter_[role] = value;
    doFilter();
    return true;
}

void SortFilterProxyListModel::clearValueOfFilter(int role)
{
    valuesOfFilter_.erase(role);
    doFilter();
}

int SortFilterProxyListModel::itemCount() const
{
    return acceptedItemCount_;
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
    if(sourceModel_)
    {
        return sourceModel_->roleNames();
    }
    return YADAW::Model::RoleNames();
}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    const auto oldCount = dstToSrc_.size();
    const auto newItemCount = last - first + 1;
    FOR_RANGE(i, first, oldCount)
    {
        dstToSrc_[srcToDst_[i]] += newItemCount;
    }
    YADAW::Util::IntegerRange<int> dstInsertRange(first, last + 1);
    srcToDst_.insert(srcToDst_.begin() + first, newItemCount, -1);
    dstToSrc_.insert(dstToSrc_.begin() + acceptedItemCount_,
        dstInsertRange.begin(), dstInsertRange.end()
    );
    auto filteredOutFirst = std::partition(
        dstToSrc_.begin() + acceptedItemCount_,
        dstToSrc_.begin() + acceptedItemCount_ + newItemCount,
        [this](int srcRow) { return this->isAccepted(srcRow); }
    );
    updateSrcToDst(acceptedItemCount_, dstToSrc_.size());
    mergeNewAcceptedItems(newItemCount);
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    removingRowsSortByDest_.reserve(last + 1 - first);
    FOR_RANGE(i, first, last + 1)
    {
        removingRowsSortByDest_.emplace_back(i, srcToDst_[i]);
    }
    using Pair = decltype(removingRowsSortByDest_)::value_type;
    std::ranges::sort(removingRowsSortByDest_,
        [](const Pair& lhs, const Pair& rhs)
        {
            return lhs.second < rhs.second;
        }
    );
}

void SortFilterProxyListModel::sourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    for(auto it = removingRowsSortByDest_.rbegin(); it != removingRowsSortByDest_.rend(); ++it)
    {
        const auto& [srcRow, dstRow] = *it;
        auto removingIsAccepted = dstRow < acceptedItemCount_;
        if(removingIsAccepted)
        {
            beginRemoveRows(QModelIndex(), dstRow, dstRow);
        }
        dstToSrc_.erase(dstToSrc_.begin() + dstRow);
        FOR_RANGE(i, dstRow, dstToSrc_.size())
        {
            --srcToDst_[dstToSrc_[i]];
        }
        if(removingIsAccepted)
        {
            --acceptedItemCount_;
            endRemoveRows();
        }
    }
    removingRowsSortByDest_.clear();
    removingRowsSortByDest_.shrink_to_fit();
    srcToDst_.erase(srcToDst_.begin() + first, srcToDst_.begin() + last + 1);
    auto removeCount = last + 1 - first;
    FOR_RANGE(i, first, srcToDst_.size())
    {
        dstToSrc_[srcToDst_[i]] -= removeCount;
    }
    validateMapping(srcToDst_, dstToSrc_);
}

void SortFilterProxyListModel::sourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doSort();
    doFilter();
    validateMapping(srcToDst_, dstToSrc_);
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
    doFilter();
    endResetModel();
}

void SortFilterProxyListModel::sortOrderModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    doSort();
}

void SortFilterProxyListModel::sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    doSort();
}

void SortFilterProxyListModel::sortOrderModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doSort();
}

void SortFilterProxyListModel::filterRoleModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    doFilter();
}

void SortFilterProxyListModel::filterRoleModelRowsRemoved(
    const QModelIndex& parent, int first, int last)
{
    doFilter();
}

void SortFilterProxyListModel::filterRoleModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doFilter();
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

bool SortFilterProxyListModel::isAccepted(int row) const
{
    if(sourceModel_)
    {
        auto index = sourceModel_->index(row);
        return (
            std::all_of(
                valuesOfFilter_.begin(), valuesOfFilter_.end(),
                [this, &index]
                    (decltype(valuesOfFilter_)::const_reference pair)
                {
                    const auto& [role, variant] = pair;
                    return sourceModel_->isPassed(
                        index, role, variant
                    );
                }
            )
        )
        &&
        (
            filterRoleModel_.itemCount() == 0
            ||
            std::any_of(filterRoleModel_.begin(), filterRoleModel_.end(),
                [this, &index]
                (const decltype(filterRoleModel_)::FilterRoleItem& pair)
                {
                    const auto& [role, caseSensitivity] = pair;
                    return sourceModel_->isSearchPassed(
                        role, index, filterString_, caseSensitivity
                    );
                }
            )
        );
    }
    return false;
}

void SortFilterProxyListModel::doSort()
{
    auto sortedCount = std::is_sorted_until(
        dstToSrc_.begin(), dstToSrc_.begin() + acceptedItemCount_,
        [this](int lhs, int rhs)
        {
            return isLess(lhs, rhs);
        }
    ) - dstToSrc_.begin();
    FOR_RANGE(i, sortedCount, acceptedItemCount_)
    {
        auto upperBound = std::upper_bound(
            dstToSrc_.begin(), dstToSrc_.begin() + i,
            dstToSrc_[i],
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        if(upperBound != dstToSrc_.begin() + i)
        {
            beginMoveRows(
                QModelIndex(), i, i,
                QModelIndex(), upperBound - dstToSrc_.begin()
            );
            auto temp = dstToSrc_[i];
            std::shift_right(upperBound, dstToSrc_.begin() + i + 1, 1);
            *upperBound = temp;
            updateSrcToDst(upperBound - dstToSrc_.begin(), i + 1);
            endMoveRows();
        }
    }
    validateMapping(srcToDst_, dstToSrc_);
}

void SortFilterProxyListModel::doFilter()
{
    auto oldAcceptedItemCount = acceptedItemCount_;
    auto removedItemCount = 0;
    FOR_RANGE0(i, oldAcceptedItemCount)
    {
        if(auto index = i - removedItemCount;
            !isAccepted(dstToSrc_[index]))
        {
            ++removedItemCount;
            beginRemoveRows(QModelIndex(), index, index);
            std::rotate(
                dstToSrc_.begin() + index,
                dstToSrc_.begin() + index + 1,
                dstToSrc_.begin() + acceptedItemCount_
            );
            FOR_RANGE(j, i, oldAcceptedItemCount)
            {
                srcToDst_[dstToSrc_[j]] = j;
            }
            --acceptedItemCount_;
            endRemoveRows();
        }
    }
    auto partitionPoint = std::partition(
        dstToSrc_.begin() + oldAcceptedItemCount,
        dstToSrc_.end(),
        [this](int srcRow) { return isAccepted(srcRow); }
    );
    auto filteredOutFirst = std::rotate(
        dstToSrc_.begin() + acceptedItemCount_,
        dstToSrc_.begin() + oldAcceptedItemCount,
        partitionPoint
    );
    updateSrcToDst(0, dstToSrc_.size());
    validateMapping(srcToDst_, dstToSrc_);
    mergeNewAcceptedItems(filteredOutFirst - dstToSrc_.begin() - acceptedItemCount_);
}

void SortFilterProxyListModel::mergeNewAcceptedItems(
    std::size_t addingItemCount)
{
    FOR_RANGE0(i, addingItemCount)
    {
        auto it = std::upper_bound(
            dstToSrc_.begin(), dstToSrc_.begin() + acceptedItemCount_,
            dstToSrc_[acceptedItemCount_],
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        auto row = it - dstToSrc_.begin();
        beginInsertRows(QModelIndex(), row, row);
        auto temp = dstToSrc_[acceptedItemCount_];
        std::shift_right(
            it, dstToSrc_.begin() + acceptedItemCount_ + 1, 1
        );
        *it = temp;
        ++acceptedItemCount_;
        FOR_RANGE(j, it - dstToSrc_.begin(), acceptedItemCount_)
        {
            srcToDst_[dstToSrc_[j]] = j;
        }
        endInsertRows();
    }
    validateMapping(srcToDst_, dstToSrc_);
}

void SortFilterProxyListModel::updateSrcToDst(int dstFirst, int dstLast)
{
    FOR_RANGE(i, dstFirst, dstLast)
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
}
}
