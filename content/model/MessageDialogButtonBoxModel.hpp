#ifndef YADAW_CONTENT_MODEL_MESSAGEDIALOGBUTTONBOXMODEL
#define YADAW_CONTENT_MODEL_MESSAGEDIALOGBUTTONBOXMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class MessageDialogButtonBoxModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        ButtonRole = Qt::UserRole,
        ButtonText,
        RoleCount
    };
    Q_ENUM(Role)
    enum ButtonRoles
    {
        ButtonRoleOk,
        ButtonRoleCancel,
        ButtonRoleApply,
        ButtonRoleYes,
        ButtonRoleNo,
        ButtonRoleYesToAll,
        ButtonRoleNoToAll,
        ButtonRoleAbort,
        ButtonRoleRetry,
        ButtonRoleIgnore
    };
    Q_ENUM(ButtonRoles)
public:
    MessageDialogButtonBoxModel(QObject* parent = nullptr);
    MessageDialogButtonBoxModel(const MessageDialogButtonBoxModel& rhs);
    ~MessageDialogButtonBoxModel();
public:
    int itemCount() const;
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override final;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE bool insert(int position, ButtonRoles role, const QString& text = QString());
    Q_INVOKABLE bool append(ButtonRoles role, const QString& text = QString());
    Q_INVOKABLE bool remove(int position, int count);
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool move(int position, int count, int newPosition);
protected:
    RoleNames roleNames() const override;
private:
    std::vector<std::pair<ButtonRoles, QString>> data_;
};
}

Q_DECLARE_METATYPE(YADAW::Model::MessageDialogButtonBoxModel)

#endif // YADAW_CONTENT_MODEL_MESSAGEDIALOGBUTTONBOXMODEL
