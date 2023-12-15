#ifndef YADAW_CONTENT_MODEL_ISORTORDERMODEL
#define YADAW_CONTENT_MODEL_ISORTORDERMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISortOrderModel: public QAbstractListModel
{
public:
    enum Role
    {
        SortRole = Qt::UserRole,
        SortOrder,
        RoleCount
    };
public:
    ISortOrderModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~ISortOrderModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool insert(int role, Qt::SortOrder sortOrder, int position) = 0;
    Q_INVOKABLE virtual bool append(int role, Qt::SortOrder sortOrder) = 0;
    Q_INVOKABLE virtual int getIndexOfRole(int role) const = 0;
    Q_INVOKABLE virtual bool remove(int index) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::SortRole, "som_sort_role"),
            std::make_pair(Role::SortOrder, "som_sort_order")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_ISORTORDERMODEL
