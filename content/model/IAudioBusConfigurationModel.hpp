#ifndef YADAW_CONTENT_MODEL_IAUDIOBUSCONFIGURATIONMODEL
#define YADAW_CONTENT_MODEL_IAUDIOBUSCONFIGURATIONMODEL

#include "model/ModelBase.hpp"
#include "model/ISortFilterListModel.hpp"

namespace YADAW::Model
{
class IAudioBusConfigurationModel: public ISortFilterListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        ChannelConfig,
        ChannelList,
        RoleCount
    };
    Q_ENUM(Role)
public:
    IAudioBusConfigurationModel(QObject* parent = nullptr): ISortFilterListModel(parent) {}
    virtual ~IAudioBusConfigurationModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool append(int channelConfig) = 0;
    Q_INVOKABLE virtual bool insert(int position, int channelConfig) = 0;
    Q_INVOKABLE virtual bool remove(int index) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name,          "abcm_name"),
            std::make_pair(Role::ChannelConfig, "abcm_channel_config"),
            std::make_pair(Role::ChannelList,   "abcm_channel_list")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUDIOBUSCONFIGURATIONMODEL
