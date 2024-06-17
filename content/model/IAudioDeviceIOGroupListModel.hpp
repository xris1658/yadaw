#ifndef YADAW_CONTENT_MODEL_IAUDIODEVICEIOGROUPLISTMODEL
#define YADAW_CONTENT_MODEL_IAUDIODEVICEIOGROUPLISTMODEL

#include "ModelBase.hpp"

#include "entity/ChannelConfig.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioDeviceIOGroupListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name,
        IsMain,
        ChannelConfig,
        ChannelCount,
        SpeakerList,
        RoleCount
    };
    using GroupType = YADAW::Entity::ChannelConfig::Config;
public:
    IAudioDeviceIOGroupListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioDeviceIOGroupListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name,             "adioglm_name"),
            std::make_pair(Role::IsMain,           "adioglm_is_main"),
            std::make_pair(Role::ChannelConfig,    "adioglm_channel_config"),
            std::make_pair(Role::SpeakerList,      "adioglm_speaker_list"),
            std::make_pair(Role::ChannelCount,     "adioglm_channel_count")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IAUDIODEVICEIOGROUPLISTMODEL
