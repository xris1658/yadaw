#include "model/TreeModelToListModel.hpp"

#include <algorithm>
#include <ranges>

namespace YADAW::Model
{
TreeModelToListModel::TreeModelToListModel(QObject* parent): QAbstractListModel(parent)
{}

TreeModelToListModel::~TreeModelToListModel()
{}

QAbstractItemModel* TreeModelToListModel::getSourceModel() const
{
    return sourceModel_;
}

void TreeModelToListModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    if(sourceModel == sourceModel_)
    {
        return;
    }
    if(sourceModel_)
    {
        QObject::disconnect(sourceModel_, nullptr, this, nullptr);
        beginResetModel();
        root_.children.clear();
    }
    if(sourceModel)
    {
        sourceModel_ = sourceModel;
        auto rootItemCount = sourceModel_->rowCount();
        root_.children.reserve(rootItemCount);
        std::generate_n(
            std::back_inserter(root_.children), rootItemCount,
            [this, offset = 0]() mutable
            {
                auto i = offset++;
                return std::make_unique<TreeNode>(
                    TreeNode {
                        .parent = &root_,
                        .sourceModelIndex = sourceModel_->index(i, 0),
                        .destIndex = i
                    }
                );
            }
        );
        endResetModel();
        QObject::connect(
            sourceModel_, &QAbstractItemModel::rowsInserted,
            this, &TreeModelToListModel::onSourceModelRowsInserted
        );
        QObject::connect(
            sourceModel_, &QAbstractItemModel::rowsRemoved,
            this, &TreeModelToListModel::onSourceModelRowsRemoved
        );
        QObject::connect(
            sourceModel_, &QAbstractItemModel::rowsMoved,
            this, &TreeModelToListModel::onSourceModelRowsMoved
        );
        QObject::connect(
            sourceModel_, &QAbstractItemModel::dataChanged,
            this, &TreeModelToListModel::onSourceModelDataChanged
        );
        QObject::connect(
            sourceModel_, &QAbstractItemModel::modelReset,
            this, &TreeModelToListModel::onSourceModelReset
        );
        QObject::connect(
            sourceModel_, &QAbstractItemModel::modelAboutToBeReset,
            this, &TreeModelToListModel::onSourceModelAboutToBeReset
        );
        maxDepth_ = 1;
    }
    // TODO
    sourceModelChanged();
}

QModelIndex TreeModelToListModel::sourceToDest(const QModelIndex& source) const
{
    if(source == QModelIndex())
    {
        return QModelIndex();
    }
    if(auto treeNode = getNode(source))
    {
        return index(treeNode->destIndex);
    }
    return {};
}

QModelIndex TreeModelToListModel::destToSource(const QModelIndex& dest) const
{
    return getNodeAndSourceIndex(dest.row()).second;
}

int TreeModelToListModel::rowCount(const QModelIndex&) const
{
    auto node = &root_;
    while(!node->children.empty())
    {
        node = node->children.back().get();
    }
    return node->destIndex + 1;
}

QVariant TreeModelToListModel::data(const QModelIndex& index, int role) const
{
    if(auto sourceIndex = destToSource(index); sourceIndex.isValid())
    {
        if(role == Role::Indent)
        {
            int ret = 0;
            for(auto i = sourceIndex; i.parent() != QModelIndex(); i = i.parent())
            {
                ++ret;
            }
            return ret;
        }
        else if(role == Role::Expanded)
        {
            auto node = getNode(sourceIndex);
            return node->status == TreeNode::Status::Expanded;
        }
        else if(role == Role::HasChildren)
        {
            auto node = getNode(sourceIndex);
            return (sourceModel_->flags(sourceIndex) & Qt::ItemFlag::ItemNeverHasChildren) == 0
                && sourceModel_->hasChildren(sourceIndex);
        }
        else if(role == Role::HasSibling)
        {
            auto node = getNode(sourceIndex);
            return node->parent->children.size() > 1;
        }
        else if(role == Role::ModelIndex)
        {
            return sourceIndex;
        }
        return sourceModel_->data(sourceIndex, role);
    }
    return {};
}

bool TreeModelToListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role >= Role::Indent && role < Qt::UserRole)
    {
        return false;
    }
    if(auto sourceIndex = destToSource(index); sourceIndex.isValid())
    {
        return sourceModel_->setData(sourceIndex, value, role);
    }
    return false;
}

void TreeModelToListModel::expand(int destIndex)
{
}

void TreeModelToListModel::expandRecursively(int destIndex)
{
}

void TreeModelToListModel::collapse(int destIndex)
{
}

void TreeModelToListModel::collapseRecursively(int destIndex)
{
}

int TreeModelToListModel::expandToIndex(const QModelIndex& sourceIndex)
{
    return -1;
}

RoleNames TreeModelToListModel::roleNames() const
{
    auto ret = sourceModel_? sourceModel_->roleNames(): RoleNames();
    ret.insert(Role::Indent,      "tmtlm_indent");
    ret.insert(Role::Expanded,    "tmtlm_expanded");
    ret.insert(Role::HasChildren, "tmtlm_has_children");
    ret.insert(Role::HasSibling,  "tmtlm_has_sibling");
    ret.insert(Role::ModelIndex,  "tmtlm_model_index");
    return ret;
}

void TreeModelToListModel::onSourceModelRowsInserted(
    const QModelIndex& sourceModelParent, int first, int last)
{
}

void TreeModelToListModel::onSourceModelRowsRemoved(
    const QModelIndex& sourceModelParent, int first, int last)
{
}

void TreeModelToListModel::onSourceModelRowsMoved(
    const QModelIndex& sourceModelFromParent, int first, int last,
    const QModelIndex& sourceModelToParent, int dest)
{
}

void TreeModelToListModel::onSourceModelDataChanged(
    const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight,
    const QList<int>& roles)
{
}

void TreeModelToListModel::onSourceModelAboutToBeReset()
{
}

void TreeModelToListModel::onSourceModelReset()
{
}

const TreeModelToListModel::TreeNode* TreeModelToListModel::getNode(const QModelIndex& sourceIndex) const
{
    if(sourceIndex == QModelIndex())
    {
        return nullptr;
    }
    std::vector<int> indices;
    indices.reserve(maxDepth_);
    for(auto index = sourceIndex; index.parent() != QModelIndex(); index = index.parent())
    {
        indices.emplace_back(index.row());
    }
    const auto* ret = &root_;
    for(auto it = indices.rbegin(); it != indices.rend(); ++it)
    {
        if(const auto& children = ret->children; (*it) < children.size())
        {
            ret = children[*it].get();
        }
        else
        {
            return nullptr;
        }
    }
    return ret;
}

std::pair<const TreeModelToListModel::TreeNode*, QModelIndex>
TreeModelToListModel::getNodeAndSourceIndex(int destIndex) const
{
    if(destIndex >= 0)
    {
        QModelIndex sourceParent;
        auto* children = &root_.children;
        while(!children->empty())
        {
            auto lowerBound = std::lower_bound(
                children->rbegin(), children->rend(), destIndex,
                [](const std::unique_ptr<TreeNode>& lhs, int rhs)
                {
                    return lhs->destIndex > rhs;
                }
            );
            if(lowerBound == children->rend())
            {
                assert(false);
                break;
            }
            else if((*lowerBound)->destIndex == destIndex)
            {
                return {
                    lowerBound->get(),
                    sourceModel_->index(
                        children->size() - 1 - (lowerBound - children->rbegin()
                    ), 0, sourceParent)
                };
            }
            else
            {
                sourceParent = sourceModel_->index(
                    children->size() - 1 - (lowerBound - children->rbegin()), 0, sourceParent
                );
                children = &((*lowerBound)->children);
            }
        }
    }
    return {nullptr, QModelIndex()};
}
}
