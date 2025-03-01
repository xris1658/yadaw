#ifndef YADAW_CONTENT_MODEL_IAudioIOPositionItemModel
#define YADAW_CONTENT_MODEL_IAudioIOPositionItemModel

#include "ISortFilterItemModel.hpp"
#include "ModelBase.hpp"

namespace YADAW::Model
{
class IAudioIOPositionItemModel: public ISortFilterItemModel
{
    Q_OBJECT
public:
    enum Role
    {
        Position = Qt::UserRole,
        IsTreeNode,
        TreeName,
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
    IAudioIOPositionItemModel(QObject *parent = nullptr): ISortFilterItemModel(parent) {}
    virtual ~IAudioIOPositionItemModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex &) const override final { return columnCount(); }
public:
    virtual Q_INVOKABLE QModelIndex findIndexByID(const QString &id) const = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Position,      "aiopim_position"),
            std::make_pair(Role::IsTreeNode,    "aiopim_is_tree_node"),
            std::make_pair(Role::TreeName,      "aiopim_tree_name"),
            std::make_pair(Role::ChannelConfig, "aiopim_channel_config"),
            std::make_pair(Role::ChannelCount,  "aiopim_channel_count")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAudioIOPositionItemModel