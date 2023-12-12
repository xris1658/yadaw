#ifndef YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
#define YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL

#include "model/ISortFilterProxyListModel.hpp"

#include "model/SortOrderModel.hpp"

#include <QList>

namespace YADAW::Model
{
class SortFilterProxyListModel: public ISortFilterProxyListModel
{
    Q_OBJECT
public:
    SortFilterProxyListModel(IComparableListModel& sourceModel, QObject* parent = nullptr);
    ~SortFilterProxyListModel() override;
public:
    IComparableListModel* sourceModel();
    const IComparableListModel* sourceModel() const;
    ISortOrderModel* sortOrderModel();
    const ISortOrderModel* sortOrderModel() const;
private slots:
    void sourceModelRowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
    void sourceModelRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void sourceModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void sourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sortOrderModelRowsInserted(const QModelIndex& parent, int first, int last);
    void sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void sortOrderModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
private:
    IComparableListModel* sourceModel_;
    SortOrderModel sortOrderModel_;
    std::vector<int> srcToDst_;
    std::vector<int> dstToSrc_;
};
}

#endif //YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
