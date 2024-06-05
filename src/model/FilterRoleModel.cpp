#include "model/FilterRoleModel.hpp"

namespace YADAW::Model
{
FilterRoleModel::FilterRoleModel(QObject* parent):
    QAbstractListModel(parent)
{}

FilterRoleModel::FilterRoleModel(const FilterRoleModel& rhs):
    QAbstractListModel(rhs.parent()),
    filterRoles_(rhs.filterRoles_)
{}

FilterRoleModel::~FilterRoleModel()
{}

FilterRoleModel::DataType::iterator FilterRoleModel::begin() noexcept
{
    return filterRoles_.begin();
}

FilterRoleModel::DataType::const_iterator FilterRoleModel::begin() const noexcept
{
    return filterRoles_.begin();
}

FilterRoleModel::DataType::iterator FilterRoleModel::end() noexcept
{
    return filterRoles_.end();
}

FilterRoleModel::DataType::const_iterator FilterRoleModel::end() const noexcept
{
    return filterRoles_.end();
}

FilterRoleModel::DataType::const_iterator FilterRoleModel::cbegin() const noexcept
{
    return filterRoles_.cbegin();
}

FilterRoleModel::DataType::const_iterator FilterRoleModel::cend() const noexcept
{
    return filterRoles_.cend();
}

int FilterRoleModel::itemCount() const
{
    return filterRoles_.size();
}

OptionalRef<const std::pair<int, Qt::CaseSensitivity>> FilterRoleModel::at(std::size_t index) const
{
    if(index < itemCount())
    {
        return {filterRoles_[index]};
    }
    return std::nullopt;
}

OptionalRef<std::pair<int, Qt::CaseSensitivity>> FilterRoleModel::at(std::size_t index)
{
    if(index < itemCount())
    {
        return {filterRoles_[index]};
    }
    return std::nullopt;
}

const std::pair<int, Qt::CaseSensitivity> FilterRoleModel::operator[](std::size_t index) const
{
    return filterRoles_[index];
}

std::pair<int, Qt::CaseSensitivity> FilterRoleModel::operator[](std::size_t index)
{
    return filterRoles_[index];
}

int FilterRoleModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

int FilterRoleModel::columnCount(const QModelIndex&) const
{
    return columnCount();
}

QVariant FilterRoleModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& [filterRole, caseSensitivity] = filterRoles_[row];
        switch(role)
        {
        case Role::FilterRole:
            return QVariant::fromValue(filterRole);
        case Role::CaseSensitivity:
            return QVariant::fromValue<int>(caseSensitivity);
        }
    }
    return {};
}

bool FilterRoleModel::setFilterRole(int role, bool filter, Qt::CaseSensitivity caseSensitivity)
{
    auto it = std::find_if(filterRoles_.begin(), filterRoles_.end(),
        [role](const std::pair<int, Qt::CaseSensitivity>& pair)
        {
            const auto& [filterRole, _] = pair;
            return filterRole == role;
        }
    );
    if(filter)
    {
        auto row = it - filterRoles_.begin();
        if(it != filterRoles_.end())
        {
            auto& [filterRole, oldCaseSensitivity] = *it;
            if(oldCaseSensitivity != caseSensitivity)
            {
                oldCaseSensitivity = caseSensitivity;
                dataChanged(index(row), index(row), {Role::CaseSensitivity});
            }
        }
        else
        {
            beginInsertRows(QModelIndex(), row, row);
            filterRoles_.emplace_back(role, caseSensitivity);
            endInsertRows();
        }
        return true;
    }
    else
    {
        if(it != filterRoles_.end())
        {
            auto row = it - filterRoles_.begin();
            beginRemoveRows(QModelIndex(), row, row);
            filterRoles_.erase(it);
            endRemoveRows();
            return true;
        }
        return false;
    }
}

void FilterRoleModel::clear()
{
    if(auto size = filterRoles_.size(); size != 0)
    {
        beginRemoveRows(QModelIndex(), 0, size - 1);
        filterRoles_.clear();
        endRemoveRows();
    }
}

RoleNames FilterRoleModel::roleNames() const
{
    static RoleNames ret
    {
        std::make_pair(Role::FilterRole, "frm_filter_role"),
        std::make_pair(Role::CaseSensitivity, "frm_case_sensitivity")
    };
    return ret;
}
}