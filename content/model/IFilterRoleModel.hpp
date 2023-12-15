#ifndef YADAW_CONTENT_MODEL_IFILTERROLEMODEL
#define YADAW_CONTENT_MODEL_IFILTERROLEMODEL

#include "ModelBase.hpp"

#include <QAbstractItemModel>

namespace YADAW::Model
{
class IFilterRoleModel: public QAbstractListModel
{
public:
    enum Role
    {
        FilterRole = Qt::UserRole,
        CaseSensitivity,
        RoleCount
    };
    IFilterRoleModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IFilterRoleModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool setFilterRole(int role, bool filter,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::FilterRole, "frm_filter_role"),
            std::make_pair(Role::CaseSensitivity, "frm_case_sensitivity")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IFILTERROLEMODEL
