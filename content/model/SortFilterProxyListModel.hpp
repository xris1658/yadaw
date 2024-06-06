#ifndef YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
#define YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL

#include "ModelBase.hpp"
#include "model/FilterRoleModel.hpp"
#include "model/SortOrderModel.hpp"

#include <QAbstractListModel>
#include <QMetaObject>
#include <QList>
#include <QQmlEngine>

#include <array>

namespace YADAW::Model
{
class SortFilterProxyListModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ getFilterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(ISortFilterListModel* sourceModel READ getSourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(SortOrderModel sortOrderModel READ getSortOrderModel)
    Q_PROPERTY(FilterRoleModel filterRoleModel READ getFilterRoleModel)
public:
    SortFilterProxyListModel(QObject* parent = nullptr);
    SortFilterProxyListModel(const SortFilterProxyListModel& rhs);
    SortFilterProxyListModel(ISortFilterListModel& sourceModel, QObject* parent = nullptr);
    ~SortFilterProxyListModel() override;
public:
    Q_INVOKABLE ISortFilterListModel* getSourceModel();
    const ISortFilterListModel* getSourceModel() const;
    void setSourceModel(ISortFilterListModel* model);
    Q_INVOKABLE SortOrderModel* getSortOrderModel();
    const SortOrderModel* getSortOrderModel() const;
    Q_INVOKABLE FilterRoleModel* getFilterRoleModel();
    const FilterRoleModel* getFilterRoleModel() const;
    Q_INVOKABLE QString getFilterString() const;
    Q_INVOKABLE void setFilterString(const QString& filterString);
    Q_INVOKABLE int mapToSource(int destIndex);
    Q_INVOKABLE int mapFromSource(int sourceIndex);
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
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
protected:
    RoleNames roleNames() const override;
signals:
    void sourceModelChanged();
    void filterStringChanged();
private slots:
    void sourceModelRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void sourceModelRowsRemoved(const QModelIndex& parent, int first, int last);
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
    bool isAccepted(int row) const;
    void doSort();
    void doFilter();
    void mergeNewAcceptedItems(std::vector<int>::iterator filteredOutFirst);
private:
    ISortFilterListModel* sourceModel_ = nullptr;
    FilterRoleModel filterRoleModel_;
    SortOrderModel sortOrderModel_;
    std::vector<int> srcToDst_;
    std::vector<int> dstToSrc_;
    int acceptedItemCount_;
    QString filterString_;
    std::map<int, QVariant> valuesOfFilter_;
    std::array<QMetaObject::Connection, 6> connections_;
};
}

Q_DECLARE_METATYPE(YADAW::Model::SortFilterProxyListModel)

#endif //YADAW_SRC_MODEL_SORTFILTERPROXYLISTMODEL
