#ifndef YADAW_CONTENT_MODEL_IAUDIOBUSCHANNELLISTMODEL
#define YADAW_CONTENT_MODEL_IAUDIOBUSCHANNELLISTMODEL

#include "model/ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioBusChannelListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        DeviceIndex = Qt::UserRole,
        ChannelIndex,
        RoleCount
    };
public:
    IAudioBusChannelListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioBusChannelListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::DeviceIndex, "abclm_device_index"),
            std::make_pair(Role::ChannelIndex, "abclm_channel_index")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IAUDIOBUSCHANNELLISTMODEL
