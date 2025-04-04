#ifndef YADAW_CONTENT_MODEL_IASSETDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_IASSETDIRECTORYLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>
#include <QString>
#include <QUrl>

#include <utility>

namespace YADAW::Model
{
class IAssetDirectoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Name,
        DefaultName,
        FileTree,
        RoleCount
    };
public:
    IAssetDirectoryListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAssetDirectoryListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
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
            std::make_pair(Role::Id,          "adlm_id"),
            std::make_pair(Role::Path,        "adlm_path"),
            std::make_pair(Role::Name,        "adlm_name"),
            std::make_pair(Role::DefaultName, "adlm_default_name"),
            std::make_pair(Role::FileTree,    "adlm_file_tree")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IASSETDIRECTORYLISTMODEL
