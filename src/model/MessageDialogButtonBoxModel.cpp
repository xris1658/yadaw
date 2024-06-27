#include "model/MessageDialogButtonBoxModel.hpp"

namespace YADAW::Model
{
MessageDialogButtonBoxModel::MessageDialogButtonBoxModel(QObject* parent):
    QAbstractListModel(parent)
{}

MessageDialogButtonBoxModel::MessageDialogButtonBoxModel(
    const MessageDialogButtonBoxModel& rhs):
    QAbstractListModel(rhs.parent()),
    data_(rhs.data_)
{
}

MessageDialogButtonBoxModel::~MessageDialogButtonBoxModel()
{}

int MessageDialogButtonBoxModel::itemCount() const
{
    return data_.size();
}

int MessageDialogButtonBoxModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int MessageDialogButtonBoxModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant MessageDialogButtonBoxModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::ButtonRole:
        {
            return QVariant::fromValue<int>(data_[row].first);
        }
        case Role::ButtonText:
        {
            return QVariant::fromValue<QString>(data_[row].second);
        }
        }
    }
    return QVariant();
}

bool MessageDialogButtonBoxModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::ButtonRole:
        {
            data_[row].first = value.value<ButtonRoles>();
            dataChanged(index, index, {Role::ButtonRole});
            return true;
        }
        case Role::ButtonText:
        {
            data_[row].second = value.value<QString>();
            dataChanged(index, index, {Role::ButtonText});
            return true;
        }
        }
    }
    return false;
}

bool MessageDialogButtonBoxModel::insert(int position, ButtonRoles role, const QString& text)
{
    if(position >= 0 && position <= itemCount())
    {
        beginInsertRows(QModelIndex(), position, position);
        data_.insert(data_.begin() + position, std::make_pair(role, text));
        return true;
    }
    return false;
}

bool MessageDialogButtonBoxModel::append(ButtonRoles role, const QString& text)
{
    auto count = itemCount();
    beginInsertRows(QModelIndex(), count, count);
    data_.insert(data_.end(), std::make_pair(role, text));
    endInsertRows();
    return true;
}

bool MessageDialogButtonBoxModel::remove(int position, int count)
{
    if(count > 0 && position >= 0 && position + count <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + count - 1);
        data_.erase(
            data_.begin() + position,
            data_.begin() + position + count
        );
        endRemoveRows();
        return true;
    }
    return false;
}

void MessageDialogButtonBoxModel::clear()
{
    remove(0, itemCount());
}

bool MessageDialogButtonBoxModel::move(int position, int count, int newPosition)
{
    if(newPosition < position)
    {
        beginMoveRows(
            QModelIndex(), position, position + count - 1,
            QModelIndex(), newPosition
        );
        std::rotate(
            data_.begin() + newPosition,
            data_.begin() + position,
            data_.begin() + position + count
        );
        endMoveRows();
        return true;
    }
    else if(newPosition > position + count)
    {
        beginMoveRows(
            QModelIndex(), position, position + count - 1,
            QModelIndex(), newPosition
        );
        std::rotate(
            data_.begin() + position,
            data_.begin() + position + count,
            data_.begin() + newPosition
        );
        endMoveRows();
    }
    return false;
}

RoleNames MessageDialogButtonBoxModel::roleNames() const
{
    static RoleNames ret
    {
        std::make_pair(Role::ButtonRole, "mdbbm_button_role"),
        std::make_pair(Role::ButtonText, "mdbbm_button_text")
    };
    return ret;
}
}
