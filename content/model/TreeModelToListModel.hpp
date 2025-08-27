#ifndef YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL
#define YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL

#include "ModelBase.hpp"

#include "util/OptionalUtil.hpp"

#include <QAbstractItemModel>
#include <QAbstractListModel>

#include <cstdint>

namespace YADAW::Model
{
class TreeModelToListModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* sourceModel READ getSourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
private:
    struct TreeNode
    {
        enum Status: std::uint8_t
        {
            Unchecked,
            NotExpanded,
            Expanded
        };
        TreeNode* parent = nullptr;
        QModelIndex sourceModelIndex;
        std::vector<std::unique_ptr<TreeNode>> children;
        int destIndex = 0;
        Status status = Status::Unchecked;
        void dump();
        void dump(std::uint32_t indent);
    };
public:
    enum Role // Copied from `QQmlTreeModelToTableModel` TODO: Use attached properties instead of this
    {
        Indent = Qt::UserRole - 5,
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
    QModelIndex sourceToDest(const QModelIndex& source) const;
    QModelIndex destToSource(const QModelIndex& dest) const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE void expand(int destIndex);
    Q_INVOKABLE void expandRecursively(int destIndex);
    Q_INVOKABLE void collapse(int destIndex);
    Q_INVOKABLE void collapseRecursively(int destIndex);
    Q_INVOKABLE void toggleExpanded(int destIndex);
    // Returns the dest index used for `ListView.positionViewAtIndex`
    Q_INVOKABLE int  expandToIndex(const QModelIndex& sourceIndex);
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
    const TreeNode* getNode(const QModelIndex& sourceIndex, bool onlyIfExpanded = true) const;
    TreeNode* getNode(const QModelIndex& sourceIndex, bool onlyIfExpanded = true);
    std::pair<const TreeNode*, QModelIndex> getNodeAndSourceIndex(int destIndex) const;
    std::pair<TreeNode*, QModelIndex> getNodeAndSourceIndex(int destIndex);
    void bumpRowCountAfter(TreeNode& node, int rowCount);
    void bumpRowCount(TreeNode& node, int rowCount);
private:
    QAbstractItemModel* sourceModel_ = nullptr;
    TreeNode root_ {
        .sourceModelIndex = QModelIndex(),
        .destIndex = -1,
        .status = TreeNode::Status::Expanded
    };
    mutable std::uint32_t maxDepth_ = 0;
};
}

Q_DECLARE_METATYPE(YADAW::Model::TreeModelToListModel)

#endif // YADAW_CONTENT_MODEL_TREEMODELTOLISTMODEL