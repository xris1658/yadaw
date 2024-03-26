#include "IntegerModel.hpp"

#include <algorithm>

IntegerModel::IntegerModel(QObject* parent):
    YADAW::Model::ISortFilterListModel(parent)
{}

IntegerModel::~IntegerModel()
{}

int IntegerModel::itemCount() const
{
    return data_.size();
}

int IntegerModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant IntegerModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount() && role == Role::Value)
    {
        return QVariant::fromValue(data_[row]);
    }
    return {};
}

bool IntegerModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount() && role == Role::Value)
    {
        data_[row] = value.value<int>();
        dataChanged(this->index(row), this->index(row), {Role::Value});
        return true;
    }
    return false;
}

bool IntegerModel::isComparable(int roleIndex) const
{
    return true;
}

bool IntegerModel::isFilterable(int roleIndex) const
{
    return false;
}

bool IntegerModel::isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return data_[lhs.row()] < data_[rhs.row()];
}

bool IntegerModel::isPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
    Qt::CaseSensitivity caseSensitivity) const
{
    return true;
}

bool IntegerModel::isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const
{
    return true;
}

bool IntegerModel::insert(int position, const QList<int>& data)
{
    if(position >= 0 && position <= itemCount())
    {
        beginInsertRows(QModelIndex(), position, position + data.size() - 1);
        std::copy(data.begin(), data.end(),
            std::inserter(data_, data_.begin() + position));
        endInsertRows();
        return true;
    }
    return false;
}

bool IntegerModel::append(const QList<int>& data)
{
    return insert(itemCount(), data);
}

bool IntegerModel::remove(int position, int count)
{
    if(position >= 0 && position < itemCount()
        && position + count <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + count - 1);
        data_.erase(data_.begin() + position, data_.begin() + position + count);
        endRemoveRows();
        return true;
    }
    return false;
}

void IntegerModel::clear()
{
    remove(0, itemCount());
}
