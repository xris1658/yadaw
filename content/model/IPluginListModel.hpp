#ifndef YADAW_CONTENT_MODEL_IPLUGINLISTMODEL
#define YADAW_CONTENT_MODEL_IPLUGINLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IPluginListModel: public QAbstractListModel
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
    IPluginListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IPluginListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void clear() = 0;
    Q_INVOKABLE virtual void asyncUpdate() = 0;
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

#endif //YADAW_CONTENT_MODEL_IPLUGINLISTMODEL
