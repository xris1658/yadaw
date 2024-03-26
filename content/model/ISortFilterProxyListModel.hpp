#ifndef YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
#define YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL

#include "FilterRoleModel.hpp"
#include "ISortFilterListModel.hpp"
#include "SortOrderModel.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISortFilterProxyListModel: public QAbstractListModel
{
Q_OBJECT
public:
    ISortFilterProxyListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
public:
    Q_INVOKABLE virtual ISortFilterListModel* sourceModel() = 0;
    virtual const ISortFilterListModel* sourceModel() const = 0;
    Q_INVOKABLE virtual SortOrderModel* getSortOrderModel() = 0;
    virtual const SortOrderModel* getSortOrderModel() const = 0;
    Q_INVOKABLE virtual FilterRoleModel* getFilterRoleModel() = 0;
    virtual const FilterRoleModel* getFilterRoleModel() const = 0;
    Q_INVOKABLE virtual QString& getFilterString() = 0;
    virtual const QString& getFilterString() const = 0;
    Q_INVOKABLE virtual void setFilterString(const QString& filterString) = 0;
    Q_INVOKABLE virtual bool insertSortOrder(int role, Qt::SortOrder sortOrder, int position) = 0;
    Q_INVOKABLE virtual bool appendSortOrder(int role, Qt::SortOrder sortOrder) = 0;
    Q_INVOKABLE virtual int getSortIndexOfRole(int role) const = 0;
    Q_INVOKABLE virtual bool removeSortOrder(int index) = 0;
    Q_INVOKABLE virtual void clearSortOrder() = 0;
    Q_INVOKABLE virtual bool setFilter(int role, bool filterEnabled,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) = 0;
    Q_INVOKABLE virtual void clearFilter() = 0;
    Q_INVOKABLE virtual QVariant valueOfFilter(int role) const = 0;
    Q_INVOKABLE virtual bool setValueOfFilter(int role, const QVariant& value) = 0;
    Q_INVOKABLE virtual void clearValueOfFilter(int role) = 0;
signals:
    void filterStringChanged();
};
}

#endif //YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
