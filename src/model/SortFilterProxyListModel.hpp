#ifndef YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
#define YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL

#include "model/ISortFilterProxyListModel.hpp"

#include "model/FilterRoleModel.hpp"
#include "model/SortOrderModel.hpp"

#include <QList>

namespace YADAW::Model
{
class SortFilterProxyListModel: public ISortFilterProxyListModel
{
    Q_OBJECT
public:
    SortFilterProxyListModel(ISortFilterListModel& sourceModel, QObject* parent = nullptr);
    ~SortFilterProxyListModel() override;
public:
    ISortFilterListModel* sourceModel();
    const ISortFilterListModel* sourceModel() const;
    ISortOrderModel* sortOrderModel();
    const ISortOrderModel* sortOrderModel() const;
    IFilterRoleModel* filterRoleModel();
    const IFilterRoleModel* filterRoleModel() const;
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&index, int role) const override;
    bool setData(const QModelIndex&index, const QVariant& value, int role) override;
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
    bool isLess(int lhs, int rhs) const;
    bool isPassed(int row, const QString& string) const;
    void doSort();
    void doFilter(const QString& string);
private:
    ISortFilterListModel* sourceModel_;
    FilterRoleModel filterRoleModel_;
    SortOrderModel sortOrderModel_;
    std::vector<int> srcToDst_;
    std::vector<int> dstToSrc_;
};
}

#endif //YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
