#ifndef YADAW_CONTENT_MODEL_IDEVICEIOGROUPLISTMODEL
#define YADAW_CONTENT_MODEL_IDEVICEIOGROUPLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IDeviceIOGroupListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name,
        IsMain,
        RoleCount
    };
public:
    IDeviceIOGroupListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IDeviceIOGroupListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name, "dioglm_name"),
            std::make_pair(Role::IsMain, "dioglm_is_main")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IDEVICEIOGROUPLISTMODEL
