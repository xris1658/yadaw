#ifndef YADAW_CONTENT_MODEL_ISENDLISTMODEL
#define YADAW_CONTENT_MODEL_ISENDLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISendListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Destination = Qt::UserRole,
        IsPreFader,
        Enabled,
        Volume,
        Panning,
        Inverted,
        RoleCount
    };
public:
    ISendListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~ISendListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    bool append();
    bool remove(int position, int removeCount);
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Destination, "slm_destination"),
            std::make_pair(Role::IsPreFader,  "slm_is_pre_fader"),
            std::make_pair(Role::Enabled,     "slm_enabled"),
            std::make_pair(Role::Volume,      "slm_volume"),
            std::make_pair(Role::Panning,     "slm_panning"),
            std::make_pair(Role::Inverted,    "slm_inverted")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_ISENDLISTMODEL
