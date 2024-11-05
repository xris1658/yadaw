#ifndef YADAW_SRC_MODEL_SORTORDERMODEL
#define YADAW_SRC_MODEL_SORTORDERMODEL

#include "ModelBase.hpp"

#include "model/ISortFilterListModel.hpp"
#include "util/OptionalUtil.hpp"

#include <QAbstractListModel>

#include <vector>

namespace YADAW::Model
{
class SortOrderModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        SortRole = Qt::UserRole,
        SortOrder,
        RoleCount
    };
public:
    SortOrderModel(QObject* parent = nullptr);
    SortOrderModel(const SortOrderModel& rhs);
    ~SortOrderModel();
public:
    int itemCount() const;
    OptionalRef<const std::pair<int, Qt::SortOrder>> at(std::size_t index) const;
    OptionalRef<std::pair<int, Qt::SortOrder>> at(std::size_t index);
    const std::pair<int, Qt::SortOrder>& operator[](std::size_t index) const;
    std::pair<int, Qt::SortOrder>& operator[](std::size_t index);
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override final;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE bool insert(int role, Qt::SortOrder sortOrder, int position);
    Q_INVOKABLE bool append(int role, Qt::SortOrder sortOrder);
    Q_INVOKABLE int getIndexOfRole(int role) const;
    Q_INVOKABLE bool remove(int index);
    Q_INVOKABLE void clear();
protected:
    RoleNames roleNames() const override;
private:
    std::vector<std::pair<int, Qt::SortOrder>> sortOrder_;
};
}

#endif //YADAW_SRC_MODEL_SORTORDERMODEL
