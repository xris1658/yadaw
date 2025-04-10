#include "FileTreeModel.hpp"

#include "util/QmlUtil.hpp"

#include <algorithm>

namespace YADAW::Model
{
FileTreeModel::FileTreeModel(const QString& path, QObject* parent):
    IFileTreeModel(parent),
    rootNode_(std::make_unique<Impl::TreeNode<List>>(QDir(path)))
{
    YADAW::Util::setCppOwnership(*this);
    rootNode_->data().fillDirEnd();
}

FileTreeModel::~FileTreeModel()
{
}

bool FileTreeModel::hasChildren(const QModelIndex& parent) const
{
    if(parent == QModelIndex())
    {
        return true;
    }
    auto* node = reinterpret_cast<Impl::TreeNode<List>*>(
        parent.internalPointer());
    if(node == nullptr)
    {
        node = rootNode_.get();
    }
    const auto& [list, dirCount] = node->data();
    auto row = parent.row();
    return row >= 0 && row < dirCount;
}

int FileTreeModel::rowCount(const QModelIndex& parent) const
{
    if(parent == QModelIndex())
    {
        auto node = rootNode_.get();
        const auto& [list, dirCount] = node->data();
        if(!node->childrenInitialized())
        {
            node->initializeChildren();
            for(std::size_t i = 0; i < dirCount; ++i)
            {
                auto child = Impl::TreeNode<List>::create(
                    node, list[i].absoluteFilePath()
                );
                child->data().fillDirEnd();
                node->addChild(child);
            }
        }
        return rootNode_->data().list.size();
    }
    auto* node = reinterpret_cast<Impl::TreeNode<List>*>(
        parent.internalPointer());
    const auto& [list, dirCount] = node->data();
    auto row = parent.row();
    if(row >= 0)
    {
        if(row < dirCount)
        {
            if(!node->childrenInitialized())
            {
                node->initializeChildren();
                for(std::size_t i = 0; i != dirCount; ++i)
                {
                    auto child = Impl::TreeNode<List>::create(
                        node, list[i].absoluteFilePath()
                    );
                    child->data().fillDirEnd();
                    node->addChild(child);
                }
            }
            const auto& childList = (*(node->childBegin() + parent.row()))->data().list;
            return childList.size();
        }
    }
    return 0;
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    auto* node = reinterpret_cast<Impl::TreeNode<List>*>(
        parent.internalPointer());
    if(node == nullptr)
    {
        node = rootNode_.get();
    }
    else
    {
        node = *(node->childBegin() + parent.row());
    }
    return createIndex(row, column, node);
}

QVariant FileTreeModel::data(const QModelIndex& index, int role) const
{
    auto data = reinterpret_cast<Impl::TreeNode<List>*>(index.internalPointer());
    if(!data)
    {
        data = rootNode_.get();
    }
    auto row = index.row();
    const auto& [list, dirCount] = data->data();
    if(row >= 0 && row < list.size())
    {
        switch(role)
        {
        case Role::Name:
        {
            return QVariant::fromValue<QString>(list[row].fileName());
        }
        case Role::Path:
        {
            return QVariant::fromValue<QString>(list[row].absoluteFilePath());
        }
        case Role::IsDirectory:
        {
            return row < (dirCount);
        }
        }
    }
    return {};
}

QModelIndex FileTreeModel::parent(const QModelIndex& child) const
{
    auto data = reinterpret_cast<Impl::TreeNode<List>*>(child.internalPointer());
    auto parentPtr = data->parent();
    if(parentPtr)
    {
        const auto& [list, dirCount] = parentPtr->data();
        auto end = parentPtr->childEnd();
        for(auto it = parentPtr->childBegin(); it != end; ++it)
        {
            if(*it == data)
            {
                return createIndex(it - parentPtr->childBegin(), 0, parentPtr);
            }
        }
    }
    return QModelIndex();
}

bool FileTreeModel::isComparable(int roleIndex) const
{
    return roleIndex == Role::Name
        || roleIndex == Role::Path
        || roleIndex == Role::IsDirectory;
}

bool FileTreeModel::isFilterable(int roleIndex) const
{
    return false;
}

bool FileTreeModel::isSearchable(int roleIndex) const
{
    return roleIndex == Role::Name
        || roleIndex == Role::Path;
}

QList<int> FileTreeModel::treeNodeRoles() const
{
    return {Role::Name, Role::Path, Role::IsDirectory};
}

QList<int> FileTreeModel::leafNodeRoles() const
{
    return {Role::Name, Role::Path, Role::IsDirectory};
}

bool FileTreeModel::isTreeNode(const QModelIndex& index) const
{
    return data(index, Role::IsDirectory).value<bool>();
}

bool FileTreeModel::isLess(
    int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(roleIndex == Role::Name || roleIndex == Role::Path)
    {
        return data(lhs, roleIndex).value<QString>() < data(rhs, roleIndex).value<QString>();
    }
    if(roleIndex == Role::IsDirectory)
    {
        return (!data(lhs, roleIndex).value<bool>())
            && ( data(rhs, roleIndex).value<bool>());
    }
    return false;
}

bool FileTreeModel::isSearchPassed(
    int roleIndex, const QModelIndex& modelIndex, const QString& string, Qt::CaseSensitivity caseSensitivity) const
{
    if(roleIndex == Role::Name || roleIndex == Role::Path)
    {
        return data(modelIndex, roleIndex).value<QString>().contains(string, caseSensitivity);
    }
    return false;
}

bool FileTreeModel::isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const
{
    return false;
}
}
