#ifndef YADAW_CONTENT_MODEL_ICOMPARABLE
#define YADAW_CONTENT_MODEL_ICOMPARABLE

#include <QAbstractListModel>

namespace YADAW::Model
{
class IComparableListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    IComparableListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
public:
    Q_INVOKABLE virtual bool isComparable(int roleIndex) const = 0;
    Q_INVOKABLE virtual bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const = 0;
};
}

#endif //YADAW_CONTENT_MODEL_ICOMPARABLE
