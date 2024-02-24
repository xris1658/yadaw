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
    ISortOrderModel* getSortOrderModel();
    const ISortOrderModel* getSortOrderModel() const;
    IFilterRoleModel* getFilterRoleModel();
    const IFilterRoleModel* getFilterRoleModel() const;
    const QString& getFilterString() const override;
    QString& getFilterString() override;
    void setFilterString(const QString& filterString) override;
    bool insertSortOrder(int role, Qt::SortOrder sortOrder, int position) override;
    bool appendSortOrder(int role, Qt::SortOrder sortOrder) override;
    int getSortIndexOfRole(int role) const override;
    bool removeSortOrder(int index) override;
    void clearSortOrder() override;
    bool setFilter(int role, bool filterEnabled,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) override;
    void clearFilter() override;
    QVariant valueOfFilter(int role) const override;
    bool setValueOfFilter(int role, const QVariant& value) override;
    void clearValueOfFilter(int role) override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&index, int role) const override;
    bool setData(const QModelIndex&index, const QVariant& value, int role) override;
protected:
    RoleNames roleNames() const override;
private slots:
    void sourceModelRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void sourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sortOrderModelRowsInserted(const QModelIndex& parent, int first, int last);
    void sortOrderModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void sortOrderModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
    void filterRoleModelRowsInserted(const QModelIndex& parent, int first, int last);
    void filterRoleModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void filterRoleModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);
private:
    bool isLess(int lhs, int rhs) const;
    bool isAccepted(int row, const QString& string) const;
    bool isAccepted(int row, const QString& string, int filterRoleBegin, int filterRoleEnd) const;
    void doSort();
    void doFilter();
    void mergeNewAcceptedItems(std::vector<int>::iterator filteredOutFirst);
    void filterOutItems();
private:
    ISortFilterListModel* sourceModel_;
    FilterRoleModel filterRoleModel_;
    SortOrderModel sortOrderModel_;
    std::vector<int> srcToDst_;
    std::vector<int> dstToSrc_;
    std::vector<int>::iterator filteredOutFirst_;
    QString filterString_;
    std::map<int, QVariant> valuesOfFilter_;
};
}

#endif //YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
