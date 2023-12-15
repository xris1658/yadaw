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
public:
    ISortFilterProxyListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
public:
    virtual Q_INVOKABLE ISortFilterListModel* sourceModel() = 0;
    virtual const ISortFilterListModel* sourceModel() const = 0;
    virtual Q_INVOKABLE ISortOrderModel* sortOrderModel() = 0;
    virtual const ISortOrderModel* sortOrderModel() const = 0;
    virtual Q_INVOKABLE IFilterRoleModel* filterRoleModel() = 0;
    virtual const IFilterRoleModel* filterRoleModel() const = 0;
};
}

#endif //YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
