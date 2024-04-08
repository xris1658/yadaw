#ifndef YADAW_CONTENT_MODEL_IPLUGINPARAMETERLISTMODEL
#define YADAW_CONTENT_MODEL_IPLUGINPARAMETERLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IPluginParameterListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Name,
        MinValue,
        MaxValue,
        DefaultValue,
        Value,
        StepSize,
        IsDiscrete,
        IsPeriodic,
        IsVisible,
        IsReadonly,
        IsAutomatable,
        ShowAsList,
        ShowAsSwitch,
        ValueAndTextList, // IParameterValueAndTextListModel
        RoleCount
    };
public:
    IPluginParameterListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IPluginParameterListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    // Returns string converted from value as fallback.
    Q_INVOKABLE virtual double valueFromString(int index, const QString& string) const = 0;
    // Returns NaN if failed.
    Q_INVOKABLE virtual QString stringFromValue(int index, double value) const = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,               "pplm_id"),
            std::make_pair(Role::Name,             "pplm_name"),
            std::make_pair(Role::MinValue,         "pplm_min_value"),
            std::make_pair(Role::MaxValue,         "pplm_max_value"),
            std::make_pair(Role::DefaultValue,     "pplm_default_value"),
            std::make_pair(Role::Value,            "pplm_value"),
            std::make_pair(Role::StepSize,         "pplm_step_size"),
            std::make_pair(Role::IsDiscrete,       "pplm_is_discrete"),
            std::make_pair(Role::IsPeriodic,       "pplm_is_periodic"),
            std::make_pair(Role::IsVisible,        "pplm_is_visible"),
            std::make_pair(Role::IsReadonly,       "pplm_is_readonly"),
            std::make_pair(Role::IsAutomatable,    "pplm_is_automatable"),
            std::make_pair(Role::ShowAsList,       "pplm_show_as_list"),
            std::make_pair(Role::ShowAsSwitch,     "pplm_show_as_switch"),
            std::make_pair(Role::ValueAndTextList, "pplm_value_and_text_list")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IPLUGINPARAMETERLISTMODEL
