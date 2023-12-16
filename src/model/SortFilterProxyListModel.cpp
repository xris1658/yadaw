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
    dstToSrc_(sourceModel.rowCount()),
    filteredOutFirst_(dstToSrc_.end()),
    filterString_()
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
    filterStringChanged();
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

void SortFilterProxyListModel::sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    const auto oldCount = itemCount();
    const auto newItemCount = last - first + 1;
    dstToSrc_.insert(dstToSrc_.begin() + itemCount(), newItemCount, -1);
    std::iota(dstToSrc_.begin() + oldCount, dstToSrc_.begin() + oldCount + newItemCount, first);
    auto filteredOutFirst = std::stable_partition(dstToSrc_.begin() + oldCount, dstToSrc_.begin() + oldCount + newItemCount,
        [this](int row)
        {
            return isAccepted(row, filterString_);
        }
    );
    auto newAcceptedItemCount = filteredOutFirst - filteredOutFirst_;
    std::stable_sort(filteredOutFirst_, filteredOutFirst,
        [this](int lhs, int rhs)
        {
            return isLess(lhs, rhs);
        }
    );
    auto firstNewItem = filteredOutFirst_;
    FOR_RANGE0(i, newAcceptedItemCount)
    {
        auto upperBound = std::upper_bound(dstToSrc_.begin(), firstNewItem,
            *firstNewItem,
            [this](int lhs, int rhs)
            {
                return isLess(lhs, rhs);
            }
        );
        auto rotateFirst = dstToSrc_.rend() - 1 - (firstNewItem - dstToSrc_.begin());
        auto rotateNewFirst = rotateFirst + i + 1;
        const auto upperBoundIndex = upperBound - dstToSrc_.begin();
        auto rotateEnd = dstToSrc_.rend() - upperBoundIndex;
        beginInsertRows(QModelIndex(), upperBoundIndex, upperBoundIndex);
        std::rotate(rotateFirst, rotateNewFirst, rotateEnd);
        endInsertRows();
        firstNewItem = upperBound;
    }
    filteredOutFirst_ = filteredOutFirst;
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

void SortFilterProxyListModel::sourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{}

void SortFilterProxyListModel::sourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    auto first = topLeft.row();
    auto last = bottomRight.row();

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

void SortFilterProxyListModel::filterRoleModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    doFilter(first, last + 1);
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
}

void SortFilterProxyListModel::filterRoleModelRowsRemoved(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::filterRoleModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
    const QList<int>& roles)
{

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

bool SortFilterProxyListModel::isAccepted(int row, const QString& string) const
{
    return isAccepted(row, string, 0, filterRoleModel_.itemCount());
}

bool SortFilterProxyListModel::isAccepted(int row, const QString& string, int begin, int end) const
{
    return std::any_of(filterRoleModel_.cbegin() + begin, filterRoleModel_.cbegin() + end,
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

void SortFilterProxyListModel::doFilter()
{
    auto outFirst = std::stable_partition(dstToSrc_.begin(), dstToSrc_.end(),
        [this](int row)
        {
            return isAccepted(row, filterString_);
        }
    );
    std::for_each(outFirst, dstToSrc_.end(),
        [this](int row)
        {
            srcToDst_[row] = -1;
        }
    );
}

void SortFilterProxyListModel::doFilter(int begin, int end)
{
    auto outFirst = std::stable_partition(dstToSrc_.begin(), dstToSrc_.end(),
        [this, begin, end](int row)
        {
            return isAccepted(row, filterString_, begin, end);
        }
    );
    std::for_each(outFirst, dstToSrc_.end(),
        [this](int row)
        {
            srcToDst_[row] = -1;
        }
    );
}
}
