#ifndef YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL
#define YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL

#include "ISortFilterListModel.hpp"
#include "ModelBase.hpp"

namespace YADAW::Model
{
class IAudioIOPositionModel: public ISortFilterListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Position = Qt::UserRole,
        ChannelConfig,
        ChannelCount,
        RoleCount
    };
    Q_ENUM(Role)
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
    Q_ENUM(PositionType)
public:
    IAudioIOPositionModel(QObject* parent = nullptr): ISortFilterListModel(parent) {}
    virtual ~IAudioIOPositionModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    virtual Q_INVOKABLE int findIndexByID(const QString& id) const = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Position,      "aiopm_position"),
            std::make_pair(Role::ChannelConfig, "aiopm_channel_config"),
            std::make_pair(Role::ChannelCount,  "aiopm_channel_count")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUDIOIOPOSITIONMODEL
