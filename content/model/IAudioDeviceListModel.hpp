#ifndef YADAW_CONTENT_MODEL_IAUDIODEVICELISTMODEL
#define YADAW_CONTENT_MODEL_IAUDIODEVICELISTMODEL

#include "model/ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioDeviceListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Name,
        Enabled,
        ChannelCount,
        RoleCount
    };
public:
    IAudioDeviceListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioDeviceListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,           "adlm_id"),
            std::make_pair(Role::Name,         "adlm_name"),
            std::make_pair(Role::Enabled,      "adlm_enabled"),
            std::make_pair(Role::ChannelCount, "adlm_channel_count")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUDIODEVICELISTMODEL
