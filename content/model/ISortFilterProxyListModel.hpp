#ifndef YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
#define YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL

#include "IComparableListModel.hpp"
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
    virtual Q_INVOKABLE IComparableListModel* sourceModel() = 0;
    virtual const IComparableListModel* sourceModel() const = 0;
    virtual Q_INVOKABLE ISortOrderModel* sortOrderModel() = 0;
    virtual const ISortOrderModel* sortOrderModel() const = 0;
};
}

#endif //YADAW_CONTENT_MODEL_ISORTFILTERPROXYMODEL
