#ifndef YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
#define YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL

#include "ModelBase.hpp"
#include "model/FilterRoleModel.hpp"
#include "model/SortOrderModel.hpp"

#include <QAbstractListModel>
#include <QList>

namespace YADAW::Model
{
class SortFilterProxyListModel: public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SortFilterProxyListModel)
    Q_PROPERTY(QString filterString READ getFilterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(SortOrderModel sortOrderModel READ getSortOrderModel)
    Q_PROPERTY(FilterRoleModel filterRoleModel READ getFilterRoleModel)
public:
    SortFilterProxyListModel();
    SortFilterProxyListModel(const SortFilterProxyListModel& rhs);
    SortFilterProxyListModel(ISortFilterListModel& sourceModel, QObject* parent = nullptr);
    ~SortFilterProxyListModel() override;
public:
    Q_INVOKABLE ISortFilterListModel* sourceModel();
    const ISortFilterListModel* sourceModel() const;
    Q_INVOKABLE SortOrderModel* getSortOrderModel();
    const SortOrderModel* getSortOrderModel() const;
    Q_INVOKABLE FilterRoleModel* getFilterRoleModel();
    const FilterRoleModel* getFilterRoleModel() const;
    Q_INVOKABLE const QString& getFilterString() const;
    QString& getFilterString();
    Q_INVOKABLE void setFilterString(const QString& filterString);
    Q_INVOKABLE bool insertSortOrder(int role, Qt::SortOrder sortOrder, int position);
    Q_INVOKABLE bool appendSortOrder(int role, Qt::SortOrder sortOrder);
    Q_INVOKABLE int getSortIndexOfRole(int role) const;
    Q_INVOKABLE bool removeSortOrder(int index);
    Q_INVOKABLE void clearSortOrder();
    Q_INVOKABLE bool setFilter(int role, bool filterEnabled,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
    Q_INVOKABLE void clearFilter();
    Q_INVOKABLE QVariant valueOfFilter(int role) const;
    Q_INVOKABLE bool setValueOfFilter(int role, const QVariant& value);
    Q_INVOKABLE void clearValueOfFilter(int role);
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&index, int role) const override;
    bool setData(const QModelIndex&index, const QVariant& value, int role) override;
protected:
    RoleNames roleNames() const override;
signals:
    void filterStringChanged();
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
