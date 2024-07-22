#ifndef YADAW_CONTENT_MODEL_INATIVEPOPUPEVENTFILTERMODEL
#define YADAW_CONTENT_MODEL_INATIVEPOPUPEVENTFILTERMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

class QWindow;

namespace YADAW::Model
{
class INativePopupEventFilterModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        NativePopup = Qt::UserRole,
        RoleCount
    };
public:
    INativePopupEventFilterModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~INativePopupEventFilterModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool insert(QWindow* nativePopup, int index) = 0;
    Q_INVOKABLE virtual bool append(QWindow* nativePopup) = 0;
    Q_INVOKABLE virtual bool remove(QWindow* nativePopup) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::NativePopup, "npefm_native_popup")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_INATIVEPOPUPEVENTFILTERMODEL
