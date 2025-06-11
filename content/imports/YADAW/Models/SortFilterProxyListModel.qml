import QtQml.Models

ListModel {
    dynamicRoles: true
    property string filterString
    property ISortFilterListModel sourceModel
    property SortOrderModel sortOrderModel
    property FilterRoleModel filterRoleModel
}