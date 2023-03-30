#ifndef YADAW_CONTENT_MODEL_IAUDIOGRAPHDEVICELISTMODEL
#define YADAW_CONTENT_MODEL_IAUDIOGRAPHDEVICELISTMODEL

#include "model/ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioGraphDeviceListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Name,
        RoleCount
    };
public:
    IAudioGraphDeviceListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioGraphDeviceListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,   "agdlm_id"),
            std::make_pair(Role::Name, "agdlm_name")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IAUDIOGRAPHDEVICELISTMODEL
