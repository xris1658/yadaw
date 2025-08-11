#ifndef YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL
#define YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractItemModel>
#include <QAbstractListModel>

namespace YADAW::Model
{
class TreeModelToListModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* sourceModel READ getSourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
public:
    enum Role // Copied from `QQmlTreeModelToTableModel`
    {
        Depth = Qt::UserRole - 5,
        Expanded,
        HasChildren,
        HasSibling,
        ModelIndex
    };
public:
    TreeModelToListModel(QObject* parent = nullptr);
    ~TreeModelToListModel();
public:
    QAbstractItemModel* getSourceModel() const;
    void setSourceModel(QAbstractItemModel* sourceModel);
public:
    QModelIndex sourceToDest(const QModelIndex& source);
    QModelIndex destToSource(const QModelIndex& dest);
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
signals:
    void sourceModelChanged();
protected:
    RoleNames roleNames() const override;
private slots:
    void onSourceModelRowsInserted(const QModelIndex& sourceModelParent, int first, int last);
    void onSourceModelRowsRemoved(const QModelIndex& sourceModelParent, int first, int last);
    void onSourceModelRowsMoved(const QModelIndex& sourceModelFromParent, int first, int last,
        const QModelIndex& sourceModelToParent, int dest
    );
    void onSourceModelDataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight,
        const QList<int>& roles
    );
    void onSourceModelAboutToBeReset();
    void onSourceModelReset();
private:
    struct TreeNode
    {
        TreeNode* parent = nullptr;
        QModelIndex sourceModelIndex;
        std::vector<std::unique_ptr<TreeNode>> children;
    };
    QAbstractItemModel* sourceModel_ = nullptr;
    TreeNode root_;

};
}

#endif // YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL