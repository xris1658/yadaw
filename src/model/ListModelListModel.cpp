#include "model/ListModelListModel.hpp"

#include "util/IntegerRange.hpp"

#include <numeric>

namespace YADAW::Model
{
ListModelListModel::ListModelListModel(QObject* parent):
    QAbstractListModel(parent)
{}

ListModelListModel::ListModelListModel(const ListModelListModel& rhs):
    QAbstractListModel(rhs.parent()),
    models_(rhs.models_)
{}

ListModelListModel::~ListModelListModel()
{}

int ListModelListModel::itemCount() const
{
    return models_.size();
}

int ListModelListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int ListModelListModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant ListModelListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::List:
            return QVariant::fromValue<QObject*>(models_[row]);
        }
    }
    return {};
}

bool ListModelListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::List:
        {
            auto model = dynamic_cast<QAbstractListModel*>(
                value.value<QObject*>()
            );
            if(model != nullptr)
            {
                models_[row] = model;
                dataChanged(this->index(row),this->index(row), {Role::List});
                return true;
            }
        }
        }
    }
    return false;
}

bool ListModelListModel::insertList(QAbstractListModel* model, int position)
{
    if(position >= 0 && position <= itemCount() && model != nullptr)
    {
        beginInsertRows(QModelIndex(), position, position);
        models_.emplace(models_.begin() + position, model);
        endInsertRows();
        return true;
    }
    return false;
}

bool ListModelListModel::appendList(QAbstractListModel* model)
{
    if(model != nullptr)
    {
        beginInsertRows(QModelIndex(), models_.size(), models_.size());
        models_.emplace_back(model);
        endInsertRows();
        return true;
    }
    return false;
}

bool ListModelListModel::removeLists(int first, int last)
{
    if(first < last
        && first >= 0 && first < itemCount()
        && last > 0 && last <= itemCount())
    {
        beginRemoveRows(QModelIndex(), first, last - 1);
        models_.erase(models_.begin() + first, models_.begin() + last);
        endRemoveRows();
        return true;
    }
    return false;
}

bool ListModelListModel::removeList(int index)
{
    if(index >= 0 && index < itemCount())
    {
        beginRemoveRows(QModelIndex(), index, index);
        models_.erase(models_.begin() + index);
        endRemoveRows();
        return true;
    }
    return false;
}

bool ListModelListModel::moveLists(int first, int last, int newFirst)
{
    if(first < last
        && first >= 0 && first < itemCount()
        && last > 0 && last <= itemCount())
    {
        if(newFirst < first) // shift left
        {
            beginMoveRows(QModelIndex(), first, last - 1, QModelIndex(), newFirst);
            std::rotate(
                models_.begin() + newFirst,
                models_.begin() + first,
                models_.begin() + last
            );
            endMoveRows();
            return true;
        }
        else if(newFirst > last)
        {
            beginMoveRows(QModelIndex(), first, last - 1, QModelIndex(), newFirst);
            std::rotate(
                models_.begin() + first,
                models_.begin() + last,
                models_.begin() + newFirst
            );
            endMoveRows();
            return true;
        }
    }
    return false;
}

bool ListModelListModel::moveList(int index, int newIndex)
{
    return moveLists(index, index + 1, newIndex);
}
}
