#ifndef YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class PluginDirectoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Path = Qt::UserRole,
        RoleCount
    };
public:
    PluginDirectoryListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~PluginDirectoryListModel() override {}
public:
    static constexpr int columnCount() { return RoleCount - Qt::UserRole; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void append(const QString& path) = 0;
    Q_INVOKABLE virtual void append(const QUrl& url) = 0;
    Q_INVOKABLE virtual void remove(int index) = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Path, "pdlm_path")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
