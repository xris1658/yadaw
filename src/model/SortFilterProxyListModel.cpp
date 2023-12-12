#include "SortFilterProxyListModel.hpp"

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

void SortFilterProxyListModel::sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sourceModelRowsInserted(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{

}

void SortFilterProxyListModel::sourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
    const QList<int>& roles)
{

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
}