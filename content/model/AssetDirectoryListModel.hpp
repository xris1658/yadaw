#ifndef YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>
#include <QString>
#include <QUrl>

#include <utility>

namespace YADAW::Model
{
class AssetDirectoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Name,
        RoleCount
    };
public:
    AssetDirectoryListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~AssetDirectoryListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void append(const QString& path, const QString& name) = 0;
    Q_INVOKABLE virtual void append(const QUrl& url) = 0;
    Q_INVOKABLE virtual void rename(int id, const QString& name) = 0;
    Q_INVOKABLE virtual void remove(int id) = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,   "adlm_id"),
            std::make_pair(Role::Path, "adlm_path"),
            std::make_pair(Role::Name, "adlm_name")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
