#ifndef YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
#define YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL

#include "IFilterRoleModel.hpp"
#include "ISortFilterListModel.hpp"
#include "ISortOrderModel.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISortFilterProxyListModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ getFilterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(ISortOrderModel sortOrderModel READ getSortOrderModel)
    Q_PROPERTY(IFilterRoleModel filterRoleModel READ getFilterRoleModel)
public:
    ISortFilterProxyListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
public:
    virtual Q_INVOKABLE ISortFilterListModel* sourceModel() = 0;
    virtual const ISortFilterListModel* sourceModel() const = 0;
    virtual Q_INVOKABLE ISortOrderModel* getSortOrderModel() = 0;
    virtual const ISortOrderModel* getSortOrderModel() const = 0;
    virtual Q_INVOKABLE IFilterRoleModel* getFilterRoleModel() = 0;
    virtual const IFilterRoleModel* getFilterRoleModel() const = 0;
    virtual Q_INVOKABLE QString& getFilterString() = 0;
    virtual const QString& getFilterString() const = 0;
    virtual Q_INVOKABLE void setFilterString(const QString& filterString) = 0;
signals:
    void filterStringChanged();
};
}

#endif //YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
