#include "model/SortFilterProxyListModel.hpp"

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
    return false;
}

SortFilterProxyListModel::SortFilterProxyListModel(QObject* parent):
    QAbstractListModel(parent)
{
    filteredOutFirst_ = dstToSrc_.begin();
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
            filteredOutFirst_ = dstToSrc_.begin();
        }
        sourceModel_ = model;
        if(oldItemCount != 0)
        {
            endRemoveRows();
        }
        if(model)
        {
            srcToDst_.resize(sourceModel_->rowCount());
            std::iota(srcToDst_.begin(), srcToDst_.end(), 0);
            dstToSrc_.resize(sourceModel_->rowCount());
            std::iota(dstToSrc_.begin(), dstToSrc_.end(), 0);
            filteredOutFirst_ = dstToSrc_.end();
            auto newItemCount = itemCount();
            if(newItemCount != 0)
            {
                beginInsertRows(QModelIndex(), 0, itemCount() - 1);
            }
            connections_[0] = QObject::connect(
                sourceModel_, &ISortFilterListModel::rowsInserted,
                this, &SortFilterProxyListModel::sourceModelRowsInserted);
            connections_[1] = QObject::connect(
                sourceModel_, &ISortFilterListModel::rowsAboutToBeRemoved,
                this, &SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved);
            connections_[2] = QObject::connect(
                sourceModel_, &ISortFilterListModel::dataChanged,
                this, &SortFilterProxyListModel::sourceModelDataChanged);
            connections_[3] = QObject::connect(
                sourceModel_, &ISortFilterListModel::modelAboutToBeReset,
                this, &SortFilterProxyListModel::sourceModelAboutToBeReset);
            connections_[4] = QObject::connect(
                sourceModel_, &ISortFilterListModel::modelReset,
                this, &SortFilterProxyListModel::sourceModelReset);
            if(newItemCount != 0)
            {
                endInsertRows();
            }
            doFilter();
            doSort();
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

int SortFilterProxyListModel::mapToSource(int destIndex)
{
    if(destIndex >= 0 && destIndex < itemCount())
    {
        return dstToSrc_[destIndex];
    }
    return -1;
}

int SortFilterProxyListModel::mapFromSource(int sourceIndex)
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
    if(sourceModel_)
    {
        return sourceModel_->roleNames();
    }
    return YADAW::Model::RoleNames();
}

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
            return isAccepted(row);
        }
    );
    mergeNewAcceptedItems(filteredOutFirst);
}

void SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    auto oldItemCount = itemCount();
    auto removeFirst = srcToDst_.begin() + first;
    auto removeLast = srcToDst_.begin() + last + 1;
    std::vector<std::pair<std::size_t, int>> rowsToRemove;
    rowsToRemove.reserve(removeLast - removeFirst);
    for(auto it = removeFirst; it != removeLast; ++it)
    {
        rowsToRemove.emplace_back(it - srcToDst_.begin(), *it);
    }
    std::sort(rowsToRemove.begin(), rowsToRemove.end(),
        [](decltype(rowsToRemove)::const_reference lhs, decltype(rowsToRemove)::const_reference rhs)
        {
            return lhs.second < rhs.second;
        }
    );
    auto middle = std::find_if_not(rowsToRemove.begin(), rowsToRemove.end(),
        [](decltype(rowsToRemove)::const_reference value)
        {
            return value.second != -1;
        }
    );
    std::rotate(
        rowsToRemove.begin(),
        middle,
        rowsToRemove.end()
    );
    // `for(auto it = removeLast; it-- > removeFirst; )` might cause an
    // assertion failure when `removeFirst == srcToDst_.begin()`.
    // See comment of `mergeNewAcceptedItems`.
    for(auto i = rowsToRemove.size(); i-- > 0;)
    {
        auto it = rowsToRemove.begin() + i;
        const auto& [srcRow, dstRow] = *it;
        if(dstRow != -1)
        {
            beginRemoveRows(QModelIndex(), dstRow, dstRow);
            // Both expressions assigning to `filteredOutFirst_` are needed.
            // Removing either of them causes an assertion failure called
            // "vector iterators incompatible" on MSVC.
            filteredOutFirst_ = dstToSrc_.begin() + (--oldItemCount);
            dstToSrc_.erase(dstToSrc_.begin() + dstRow);
            endRemoveRows();
            filteredOutFirst_ = dstToSrc_.begin() + oldItemCount;
        }
        else
        {
            dstToSrc_.erase(
                std::remove(
                    filteredOutFirst_, dstToSrc_.end(), srcRow
                )
            );
        }
    }
    srcToDst_.erase(removeFirst, removeLast);
    filteredOutFirst_ = dstToSrc_.begin() + oldItemCount;
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::sourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    doSort();
    doFilter();
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
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
    FOR_RANGE0(i, itemCount())
    {
        auto it = dstToSrc_.begin() + i;
        auto upperBound = std::upper_bound(dstToSrc_.begin(), it, *it,
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        if(upperBound != it)
        {
            beginMoveRows(QModelIndex(), i, i, QModelIndex(), upperBound - dstToSrc_.begin());
            std::rotate(upperBound, it, it + 1);
            endMoveRows();
        }
    }
    FOR_RANGE0(i, itemCount())
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::doFilter()
{
    auto oldFilteredOutFirst = filteredOutFirst_;
    auto oldItemCount = itemCount();
    auto unchangedEnd = filteredOutFirst_;
    for(auto i = oldItemCount; i-- > 0;)
    {
        if(auto it = dstToSrc_.begin() + i; !isAccepted(*it))
        {
            unchangedEnd = it;
            beginRemoveRows(QModelIndex(), i, i);
            std::rotate(it, it + 1, filteredOutFirst_);
            --filteredOutFirst_;
            endRemoveRows();
        }
    }
    auto newItemCount = itemCount();
    auto unchangedCount = unchangedEnd - dstToSrc_.begin();
    FOR_RANGE(i, unchangedCount, newItemCount)
    {
        srcToDst_[dstToSrc_[i]] = i;
    }
    FOR_RANGE(i, newItemCount, oldItemCount)
    {
        srcToDst_[dstToSrc_[i]] = -1;
    }
    auto newFilteredOutFirst = std::partition(
        oldFilteredOutFirst, dstToSrc_.end(),
        [this](int row)
        {
            return isAccepted(row);
        }
    );
    newFilteredOutFirst = std::rotate(
        filteredOutFirst_, oldFilteredOutFirst, newFilteredOutFirst);
    mergeNewAcceptedItems(newFilteredOutFirst);
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}

void SortFilterProxyListModel::mergeNewAcceptedItems(
    std::vector<int>::iterator filteredOutFirst)
{
    auto newAcceptedItemCount = filteredOutFirst - filteredOutFirst_;
    // Some STL implementations (e.g. MSVC STL) does NOT allow an iterator which
    // is before begin() of the `std::vector`. To prevent this, we have to check
    // if no items are added to the empty list, which calculates `begin() - 1`.
    // While coding for multi-platform applications, please check the different
    // implementations of the same modules.
    if(newAcceptedItemCount != 0)
    {
        std::sort(filteredOutFirst_, filteredOutFirst,
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        auto firstNewItem = filteredOutFirst_;
        // See the comment above
        auto beforeLast = firstNewItem + newAcceptedItemCount - 1;
        FOR_RANGE0(i, newAcceptedItemCount)
        {
            auto newFirstNewIterator = std::upper_bound(
                dstToSrc_.begin(), firstNewItem, *beforeLast,
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
        FOR_RANGE0(i, itemCount())
        {
            srcToDst_[dstToSrc_[i]] = i;
        }
    }
    assert(validateMapping(srcToDst_, dstToSrc_, filteredOutFirst_));
}
}
