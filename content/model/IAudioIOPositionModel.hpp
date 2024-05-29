#ifndef YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL
#define YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioIOPositionModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        ID = Qt::UserRole,
        Name,
        Type,
        ChannelConfig,
        ChannelCount,
        RoleCount
    };
public:
    enum PositionType
    {
        HardwareAudioInput,
        PluginAuxInput,
        PluginAuxOutput,
        AudioFXChannel,
        AudioGroupChannel,
        HardwareAudioOutput
    };
public:
    IAudioIOPositionModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioIOPositionModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    virtual Q_INVOKABLE int findIndexByID(int id) const = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::ID,            "aiopm_id"),
            std::make_pair(Role::Name,          "aiopm_name"),
            std::make_pair(Role::Type,          "aiopm_type"),
            std::make_pair(Role::ChannelConfig, "aiopm_channel_config"),
            std::make_pair(Role::ChannelCount,  "aiopm_channel_count")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL
