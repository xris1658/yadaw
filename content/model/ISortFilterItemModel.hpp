#ifndef YADAW_CONTENT_MODEL_ISORTFILTERITEMMODEL
#define YADAW_CONTENT_MODEL_ISORTFILTERITEMMODEL

#include <QAbstractListModel>
#include <QList>

namespace YADAW::Model
{
class ISortFilterItemModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    ISortFilterItemModel(QObject* parent = nullptr): QAbstractItemModel(parent) {}
public:
    Q_INVOKABLE virtual bool isComparable(int roleIndex) const = 0;
    Q_INVOKABLE virtual bool isFilterable(int roleIndex) const = 0;
    Q_INVOKABLE virtual bool isSearchable(int roleIndex) const = 0;
    Q_INVOKABLE virtual QList<int> treeNodeRoles() const = 0;
    Q_INVOKABLE virtual QList<int> leafNodeRoles() const = 0;
    Q_INVOKABLE virtual bool isTreeNode(const QModelIndex& index) const = 0;
    virtual bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const = 0;
    virtual bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) const = 0;
    virtual bool isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const = 0;
signals:
    void fileterCaseSensitivityChanged();
};
}

#endif // YADAW_CONTENT_MODEL_ISORTFILTERITEMMODEL