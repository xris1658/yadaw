#ifndef YADAW_SRC_MODEL_FILTERROLEMODEL
#define YADAW_SRC_MODEL_FILTERROLEMODEL

#include "ModelBase.hpp"
#include "model/ISortFilterListModel.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Model
{
class FilterRoleModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        FilterRole = Qt::UserRole,
        CaseSensitivity,
        RoleCount
    };
    using FilterRoleItem = std::pair<int, Qt::CaseSensitivity>;
private:
    using DataType = std::vector<FilterRoleItem>;
public:
    FilterRoleModel(QObject* parent = nullptr);
    ~FilterRoleModel();
public:
    DataType::iterator begin() noexcept;
    DataType::const_iterator begin() const noexcept;
    DataType::iterator end() noexcept;
    DataType::const_iterator end() const noexcept;
    DataType::const_iterator cbegin() const noexcept;
    DataType::const_iterator cend() const noexcept;
public:
    int itemCount() const;
    OptionalRef<const std::pair<int, Qt::CaseSensitivity>> at(std::size_t index) const;
    OptionalRef<std::pair<int, Qt::CaseSensitivity>> at(std::size_t index);
    const std::pair<int, Qt::CaseSensitivity> operator[](std::size_t index) const;
    std::pair<int, Qt::CaseSensitivity> operator[](std::size_t index);
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override final;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    bool setFilterRole(int role, bool filter,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
    void clear();
protected:
    RoleNames roleNames() const override;
private:
    std::vector<std::pair<int, Qt::CaseSensitivity>> filterRoles_;
};
}

#endif //YADAW_SRC_MODEL_FILTERROLEMODEL
