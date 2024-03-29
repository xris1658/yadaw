#ifndef YADAW_CONTENT_MODEL_ICOMPARABLE
#define YADAW_CONTENT_MODEL_ICOMPARABLE

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISortFilterListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    ISortFilterListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
public:
    Q_INVOKABLE virtual bool isComparable(int roleIndex) const = 0;
    Q_INVOKABLE virtual bool isFilterable(int roleIndex) const = 0;
    Q_INVOKABLE virtual bool isSearchable(int roleIndex) const = 0;
    virtual bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const = 0;
    virtual bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) const = 0;
    virtual bool isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const = 0;
signals:
    void filterCaseSensitivityChanged();
};
}

#endif //YADAW_CONTENT_MODEL_ICOMPARABLE
