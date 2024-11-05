#ifndef YADAW_CONTENT_MODEL_IPLUGINLISTMODEL
#define YADAW_CONTENT_MODEL_IPLUGINLISTMODEL

#include "ISortFilterListModel.hpp"
#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IPluginListModel: public ISortFilterListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Uid,
        Name,
        Vendor,
        Version,
        Format,
        Type,
        RoleCount
    };
    Q_ENUM(Role)
public:
    enum PluginFormat
    {
        UnknownFormat,
        VST3,
        CLAP,
        Vestifal
    };
    Q_ENUM(PluginFormat)
    enum PluginType
    {
        UnknownType,
        MIDIEffect,
        Instrument,
        AudioEffect
    };
    Q_ENUM(PluginType)
public:
    IPluginListModel(QObject* parent = nullptr): ISortFilterListModel(parent) {}
    virtual ~IPluginListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,      "plm_id"),
            std::make_pair(Role::Path,    "plm_path"),
            std::make_pair(Role::Uid,     "plm_uid"),
            std::make_pair(Role::Name,    "plm_name"),
            std::make_pair(Role::Vendor,  "plm_vendor"),
            std::make_pair(Role::Version, "plm_version"),
            std::make_pair(Role::Format,  "plm_format"),
            std::make_pair(Role::Type,    "plm_type")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IPLUGINLISTMODEL
