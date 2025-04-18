#ifndef YADAW_SRC_MODEL_FILETREEMODEL
#define YADAW_SRC_MODEL_FILETREEMODEL

#include "model/IFileTreeModel.hpp"

#include "model/impl/TreeNode.hpp"

#include <QAbstractItemModel>
#include <QDir>
#include <QFileInfo>

#include <memory>

namespace YADAW::Model
{
class FileTreeModel: public IFileTreeModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Path,
        IsDirectory
    };
    Q_ENUM(Role)
private:
    struct List
    {
        List(const QDir& dir):
            list(
                dir.entryInfoList(
                    QDir::Filter::AllDirs
                    | QDir::Filter::NoDotAndDotDot
                    | QDir::Filter::Files
                    | QDir::Filter::Hidden,
                    QDir::SortFlag::Name
                    | QDir::SortFlag::DirsFirst
                )
            )
        {}
        void fillDirEnd()
        {
            auto dirEnd = std::partition_point(
                list.cbegin(), list.cend(),
                [](const QFileInfo& info)
                {
                    return info.isDir();
                }
            );
            dirCount = dirEnd - list.cbegin();
        }
        QFileInfoList list;
        std::size_t dirCount = 0;
    };
public:
    FileTreeModel(const QString& path, QObject* parent = nullptr);
    ~FileTreeModel() override;
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;
public:
    bool isComparable(int roleIndex) const override;
    bool isFilterable(int roleIndex) const override;
    bool isSearchable(int roleIndex) const override;
    QList<int> treeNodeRoles() const override;
    QList<int> leafNodeRoles() const override;
    bool isTreeNode(const QModelIndex& index) const override;
    bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string, Qt::CaseSensitivity caseSensitivity) const override;
    bool isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const override;
private:
    mutable std::unique_ptr<Impl::TreeNode<List>> rootNode_;
};
}

#endif // YADAW_SRC_MODEL_FILETREEMODEL
