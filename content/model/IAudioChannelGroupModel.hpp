#ifndef YADAW_CONTENT_MODEL_IAUDIOCHANNELGROUPMODEL
#define YADAW_CONTENT_MODEL_IAUDIOCHANNELGROUPMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioChannelGroupModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        ChannelType,
        Name,
        RoleCount
    };
    enum Channel
    {
        Center,
        Left,
        Right,
        SideLeft,
        SideRight,
        RearLeft,
        RearRight,
        RearCenter,
        LFE
    };
    Q_ENUM(Channel)
public:
    IAudioChannelGroupModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioChannelGroupModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::ChannelType, "acgm_channel_type"),
            std::make_pair(Role::Name, "acgm_name")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUDIOCHANNELGROUPMODEL
