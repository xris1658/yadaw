#ifndef YADAW_CONTENT_MODEL_ICLAPAUDIOBUSCONFIGURATIONLISTMODEL
#define YADAW_CONTENT_MODEL_ICLAPAUDIOBUSCONFIGURATIONLISTMODEL

#include "ISortFilterListModel.hpp"
#include "ModelBase.hpp"

namespace YADAW::Model
{
class ICLAPAudioBusConfigurationListModel: public ISortFilterListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Name,
        InputCount,
        OutputCount,
        HasMainInput,
        HasMainOutput,
        MainInputChannelConfig,
        MainInputChannelCount,
        MainOutputChannelConfig,
        MainOutputChannelCount,
        RoleCount
    };
    Q_ENUM(Role)
public:
    ICLAPAudioBusConfigurationListModel(QObject* parent = nullptr):
        ISortFilterListModel(parent) {}
    virtual ~ICLAPAudioBusConfigurationListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,                      "clapabclm_id"),
            std::make_pair(Role::Name,                    "clapabclm_name"),
            std::make_pair(Role::InputCount,              "clapabclm_input_count"),
            std::make_pair(Role::OutputCount,             "clapabclm_output_count"),
            std::make_pair(Role::HasMainInput,            "clapabclm_has_main_input"),
            std::make_pair(Role::HasMainOutput,           "clapabclm_has_main_output"),
            std::make_pair(Role::MainInputChannelConfig,  "clapabclm_main_input_channel_config"),
            std::make_pair(Role::MainInputChannelCount,   "clapabclm_main_input_channel_count"),
            std::make_pair(Role::MainOutputChannelConfig, "clapabclm_main_output_channel_config"),
            std::make_pair(Role::MainOutputChannelCount,  "clapabclm_main_output_channel_count"),
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_ICLAPAUDIOBUSCONFIGURATIONLISTMODEL