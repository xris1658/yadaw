#include "model/TreeModelToListModel.hpp"

#include <algorithm>
#include <ranges>

namespace YADAW::Model
{
void TreeModelToListModel::TreeNode::dump()
{
    std::fprintf(stderr, "[DEBUG] --- Begin dump ---\n");
    for(const auto& child: children)
    {
        child->dump(0);
    }
    std::fprintf(stderr, "[DEBUG] ---  End dump  ---\n");
}

void TreeModelToListModel::TreeNode::dump(std::uint32_t indent)
{
    std::fprintf(stderr, "[DEBUG] %*s%c %d\n",
        indent * 4, "",
        status == Status::Unchecked? 'x': status == Status::NotExpanded? 'v': '^', destIndex);
    for(const auto& child: children)
    {
        child->dump(indent + 1);
    }
}

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
        root_.children.clear();
    }
    beginResetModel();
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
    endResetModel();
    // TODO
    sourceModelChanged();
    root_.dump();
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
    if(auto [node, sourceIndex] = getNodeAndSourceIndex(index.row()); sourceIndex.isValid())
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
            return node->status == TreeNode::Status::Expanded;
        }
        else if(role == Role::HasChildren)
        {
            return (sourceModel_->flags(sourceIndex) & Qt::ItemFlag::ItemNeverHasChildren) == 0
                && sourceModel_->hasChildren(sourceIndex);
        }
        else if(role == Role::HasSibling)
        {
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
    if(!sourceModel_)
    {
        return;
    }
    if(auto [node, sourceIndex] = getNodeAndSourceIndex(destIndex);
        node && sourceIndex.isValid()
    )
    {
        if(node->status == TreeNode::Status::Unchecked)
        {
            if((!sourceModel_->flags(sourceIndex).testFlag(Qt::ItemFlag::ItemNeverHasChildren))
                && sourceModel_->hasChildren(sourceIndex)
            )
            {
                node->status = TreeNode::Status::Expanded;
                if(auto rowCount = sourceModel_->rowCount(sourceIndex); rowCount > 0)
                {
                    std::fprintf(stderr, "[DEBUG] TreeModelToListModel insert: %d\t%d\n", destIndex + 1, destIndex + rowCount);
                    std::fprintf(stderr, "[DEBUG] Bump row count: %d\n", rowCount);
                    beginInsertRows(QModelIndex(), destIndex + 1, destIndex + rowCount);
                    auto& children = node->children;
                    children.clear();
                    children.reserve(rowCount);
                    std::generate_n(
                        std::back_inserter(children), rowCount,
                        [node, offset = 0, destIndex, &sourceIndex]() mutable
                        {
                            return std::make_unique<TreeNode>(
                                TreeNode {
                                    .parent = node,
                                    .sourceModelIndex = sourceIndex.model()->index(offset, 0, sourceIndex),
                                    .children = {},
                                    .destIndex = destIndex + (++offset),
                                    .status = TreeNode::Status::Unchecked
                                }
                            );
                        }
                    );
                    decltype(maxDepth_) depth = 0;
                    for(auto n = node; n != &root_; n = n->parent)
                    {
                        ++depth;
                        bumpRowCountAfter(*n, rowCount);
                    }
                    maxDepth_ = std::max(maxDepth_, depth);
                    endInsertRows();
                    root_.dump();
                }
                dataChanged(index(destIndex), index(destIndex), {Role::Expanded});
            }
        }
        else if(node->status == TreeNode::Status::NotExpanded)
        {
            node->status = TreeNode::Status::Expanded;
            if(auto& children = node->children; !children.empty())
            {
                auto* last = children.back().get();
                while(last->status == TreeNode::Status::Expanded && !last->children.empty())
                {
                    last = last->children.back().get();
                }
                auto rowCount = last->destIndex - children.front()->destIndex + 1;
                std::fprintf(stderr, "[DEBUG] TreeModelToListModel insert: %d\t%d\n", children.front()->destIndex, last->destIndex);
                beginInsertRows(QModelIndex(), children.front()->destIndex, last->destIndex);
                decltype(maxDepth_) depth = 0;
                for(auto n = node; n != &root_; n = n->parent)
                {
                    ++depth;
                    bumpRowCountAfter(*n, rowCount);
                }
                maxDepth_ = std::max(maxDepth_, depth);
                endInsertRows();
                root_.dump();
            }
            dataChanged(index(destIndex), index(destIndex), {Role::Expanded});
        }
    }
}

void TreeModelToListModel::expandRecursively(int destIndex)
{
}

void TreeModelToListModel::collapse(int destIndex)
{
    if(!sourceModel_)
    {
        return;
    }
    if(auto [node, sourceIndex] = getNodeAndSourceIndex(destIndex);
        node && sourceIndex.isValid()
        && node->status == TreeNode::Status::Expanded
    )
    {
        node->status = TreeNode::Status::NotExpanded;
        if(const auto& children = node->children; !children.empty())
        {
            auto* last = children.back().get();
            while(last->status == TreeNode::Status::Expanded && !last->children.empty())
            {
                last = last->children.back().get();
            }
            std::fprintf(stderr, "[DEBUG] TreeModelToListModel remove: %d\t%d\n", destIndex + 1, last->destIndex);
            beginRemoveRows(QModelIndex(), destIndex + 1, last->destIndex);
            auto bumpRowCount = destIndex - last->destIndex;
            for(auto n = node; n != &root_; n = n->parent)
            {
                bumpRowCountAfter(*n, bumpRowCount);
            }
            endRemoveRows();
            root_.dump();
        }
        dataChanged(index(destIndex), index(destIndex), {Role::Expanded});
    }
}

void TreeModelToListModel::collapseRecursively(int destIndex)
{
}

void TreeModelToListModel::toggleExpanded(int destIndex)
{
    if(auto [node, sourceIndex] = getNodeAndSourceIndex(destIndex); sourceIndex.isValid())
    {
        if(node->status == TreeNode::Status::Expanded)
        {
            collapse(destIndex);
        }
        else
        {
            expand(destIndex);
        }
    }
}

int TreeModelToListModel::expandToIndex(const QModelIndex& sourceIndex)
{
    if(auto parentIndex = sourceIndex.parent(); parentIndex != QModelIndex())
    {
        std::vector<int> indices; indices.reserve(maxDepth_);
        for(auto index = parentIndex; index != QModelIndex(); index = index.parent())
        {
            indices.emplace_back(index.row());
        }
        maxDepth_ = std::max(maxDepth_, static_cast<std::uint32_t>(indices.size()) + 1);
        auto node = &root_;
        for(auto index: indices)
        {
            node = node->children[index].get();
            expand(node->destIndex);
        }
        return node->children[sourceIndex.row()]->destIndex;
    }
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
    if(auto node = getNode(sourceModelParent, false);
        node && node->status != TreeNode::Status::Unchecked)
    {
        TreeNode* lowestNotExpandedNode = &root_;
        for(auto n = node; n != &root_; n = n->parent)
        {
            if(n->status != TreeNode::Status::Expanded)
            {
                lowestNotExpandedNode = n;
                break;
            }
        }
        auto prevNode = node;
        if(first)
        {
            prevNode = prevNode->children[first - 1].get();
            while(prevNode->status == TreeNode::Status::Expanded && !node->children.empty())
            {
                prevNode = prevNode->children.back().get();
            }
        }
        auto prevDestIndex = prevNode->destIndex;
        if(lowestNotExpandedNode == &root_)
        {
            std::fprintf(
                stderr, "Dest index from (first, last): %d, %d\n",
                prevDestIndex + 1,
                prevDestIndex + 1 + last - first
            );
            beginInsertRows(
                QModelIndex(),
                prevDestIndex + 1,
                prevDestIndex + 1 + last - first
            );
        }
        std::ranges::copy(
            std::ranges::views::iota(first, last + 1) |
            std::ranges::views::transform(
                [node, sourceModelParent, destIndex = prevDestIndex + 1](int row) mutable
                {
                    return std::make_unique<TreeNode>(
                        TreeNode {
                            .parent = node,
                            .sourceModelIndex = sourceModelParent.model()->index(row, 0, sourceModelParent),
                            .destIndex = destIndex++
                        }
                    );
                }
            ),
            std::inserter(node->children, node->children.begin() + first)
        );
        for(auto it = node->children.begin() + last + 1; it != node->children.end(); ++it)
        {
            (*it)->sourceModelIndex = sourceModel_->index(
                it - node->children.begin(), 0, node->sourceModelIndex
            );
        }
        root_.dump();
        std::fprintf(stderr, "Items inserted, about to bump dest index\n");
        for(auto n = node->children[last].get(); n != lowestNotExpandedNode; n = n->parent)
        {
            bumpRowCountAfter(*n, last - first + 1);
            root_.dump();
        }
        if(lowestNotExpandedNode == &root_)
        {
            root_.dump();
            std::fprintf(stderr, "Items inserted, about to call `endInsertRows()`\n");
            endInsertRows();
        }
    }
}

void TreeModelToListModel::onSourceModelRowsRemoved(
    const QModelIndex& sourceModelParent, int first, int last)
{
    if(auto node = getNode(sourceModelParent, false);
        node && node->status != TreeNode::Status::Unchecked)
    {
        TreeNode* lowestNotExpandedNode = &root_;
        for(auto n = node; n != &root_; n = n->parent)
        {
            if(n->status != TreeNode::Status::Expanded)
            {
                lowestNotExpandedNode = n;
                break;
            }
        }
        auto prevDestIndex = first?
            node->children[first - 1]->destIndex:
            node->destIndex;
        auto destLast = node->children[last].get();
        if(lowestNotExpandedNode == &root_)
        {
            while(destLast->status == TreeNode::Status::Expanded && (!destLast->children.empty()))
            {
                destLast = destLast->children.back().get();
            }
            beginRemoveRows(
                QModelIndex(),
                prevDestIndex + 1,
                destLast->destIndex
            );
        }
        auto bump = node->children[first]->destIndex - (destLast->destIndex + 1);
        node->children.erase(
            node->children.begin() + first,
            node->children.begin() + last + 1
        );
        for(auto it = node->children.begin() + first; it != node->children.end(); ++it)
        {
            (*it)->sourceModelIndex = sourceModel_->index(
                it - node->children.begin(), 0, node->sourceModelIndex
            );
        }
        if(first == 0)
        {
            bumpRowCount(*node, bump);
        }
        else
        {
            bumpRowCountAfter(*node->children[first - 1], bump);
        }
        for(auto n = node; n != lowestNotExpandedNode; n = n->parent)
        {
            bumpRowCountAfter(*n, bump);
        }
        if(lowestNotExpandedNode == &root_)
        {
            endRemoveRows();
        }
    }
}

void TreeModelToListModel::onSourceModelRowsMoved(
    const QModelIndex& sourceModelFromParent, int first, int last,
    const QModelIndex& sourceModelToParent, int dest)
{
    if(sourceModelFromParent == sourceModelToParent)
    {
        if(auto fromNode = getNode(sourceModelFromParent, false);
            fromNode && fromNode->status != TreeNode::Status::Unchecked)
        {
            auto* lowestNotExpandedNode = &root_;
            for(auto n = fromNode; n != &root_; n = n->parent)
            {
                if(n->status != TreeNode::Status::Expanded)
                {
                    lowestNotExpandedNode = n;
                    break;
                }
            }
            auto& children = fromNode->children;
            if(lowestNotExpandedNode == &root_)
            {
                beginMoveRows(
                    QModelIndex(),
                    children[first]->destIndex, children[last]->destIndex,
                    QModelIndex(),
                    children[dest]->destIndex
                );
            }
            int rotateIndices[5] = {first, last + 1, dest, first, last + 1};
            auto* rotateIndex = rotateIndices + (first < dest? 0: 2);
            auto bumpBeforeNewMiddle = 0;
            auto bumpAfterNewMiddle = 0;
            auto* n = children[rotateIndex[2]].get();
            while(n->status == TreeNode::Status::Expanded && (!n->children.empty()))
            {
                n = n->children.back().get();
            }
            bumpBeforeNewMiddle = children[rotateIndex[1]]->destIndex - (n->destIndex + 1);
            n = children[rotateIndex[1]].get();
            while(n->status == TreeNode::Status::Expanded && (!n->children.empty()))
            {
                n = n->children.back().get();
            }
            bumpAfterNewMiddle = n->destIndex + 1 - children[rotateIndex[0]]->destIndex;
            auto newMiddle = std::rotate(
                children.begin() + rotateIndex[0],
                children.begin() + rotateIndex[1],
                children.begin() + rotateIndex[2]
            );
            for(auto it = children.begin() + rotateIndex[0]; it != newMiddle; ++it)
            {
                auto& child = **it;
                child.sourceModelIndex = sourceModel_->index(
                    it - children.begin(), 0, sourceModelFromParent
                );
                child.destIndex += bumpBeforeNewMiddle;
                bumpRowCount(child, bumpBeforeNewMiddle);
            }
            for(auto it = newMiddle; it != children.begin() + rotateIndex[2]; ++it)
            {
                auto& child = **it;
                child.sourceModelIndex = sourceModel_->index(
                    it - children.begin(), 0, sourceModelFromParent
                );
                child.destIndex += bumpAfterNewMiddle;
                bumpRowCount(child, bumpAfterNewMiddle);
            }
            if(lowestNotExpandedNode == &root_)
            {
                endMoveRows();
            }
        }
    }
    else
    {
        std::vector<int> indices; indices.reserve(maxDepth_ * 2);
        for(auto fromIndex = sourceModelFromParent; fromIndex != QModelIndex(); fromIndex = fromIndex.parent())
        {
            indices.emplace_back(fromIndex.row());
        }
        auto fromIndexCount = indices.size();
        for(auto toIndex = sourceModelToParent; toIndex != QModelIndex(); toIndex = toIndex.parent())
        {
            indices.emplace_back(toIndex.row());
        }
        auto toIndexCount = indices.size() - fromIndexCount;
        auto fromIndexRange = std::ranges::views::counted(indices.rbegin() + toIndexCount, fromIndexCount);
        auto toIndexRange = std::ranges::views::counted(indices.rbegin(), toIndexCount);
        auto [mismatchFrom, mismatchTo] = std::ranges::mismatch(fromIndexRange, toIndexRange);
        // Lexicographical compare:
        // | mismatchFrom | mismatchto | result |
        // |     end      |     1      |   <    |
        // |      1       |    end     |   >    |
        // |      1       |     2      |   <    |
        // |      2       |     1      |   >    |
        auto fromIsUpper = mismatchFrom == fromIndexRange.end()
            || (mismatchTo != toIndexRange.end() && *mismatchFrom < *mismatchTo);
        struct Range
        {
            decltype(fromIndexRange)& range;
            decltype(mismatchFrom)&   mismatch;
            decltype(mismatchFrom)    unexpandedFirst;
            decltype(mismatchFrom)    uncheckedFirst;
            TreeNode*                 unexpandedParentNode;
            TreeNode*                 uncheckedParentNode;
            Range(TreeNode* root, decltype(range) range, decltype(mismatch)& mismatch):
            range(range), mismatch(mismatch),
            unexpandedFirst(range.begin()), uncheckedFirst(range.begin()),
            unexpandedParentNode(root), uncheckedParentNode(root)
            {
                while(unexpandedParentNode->status == TreeNode::Status::Expanded)
                {
                    unexpandedParentNode = unexpandedParentNode->children[*(unexpandedFirst++)].get();
                }
                uncheckedFirst = unexpandedFirst;
                uncheckedParentNode = unexpandedParentNode;
                while(uncheckedParentNode->status != TreeNode::Status::Unchecked)
                {
                    uncheckedParentNode = uncheckedParentNode->children[*(uncheckedFirst++)].get();
                }
            }
        };
        Range ranges[2] = {
            Range { &root_, toIndexRange,   mismatchTo   },
            Range { &root_, fromIndexRange, mismatchFrom },
        };
        auto& fromRange  = ranges[0];
        auto& toRange    = ranges[1];
        auto& upperRange = ranges[ fromIsUpper];
        auto& lowerRange = ranges[!fromIsUpper];
        auto divergingIndex = QModelIndex();
        for(auto it = fromIndexRange.begin(); it != mismatchFrom; ++it)
        {
            divergingIndex = sourceModel_->index(*it, 0, divergingIndex);
        }
        if(auto divergingNode = getNode(divergingIndex)) // FIXME: handle unexpanded/unchecked nodes
        {
            for(auto it = toIndexRange.begin(); it != mismatchTo; ++it)
            {
                divergingNode = divergingNode->children[*it].get();
            }
            auto movingFirst = getNode(sourceModel_->index(first, 0, sourceModelFromParent)); // FIXME: handle unexpanded/unchecked nodes
            auto movingLast  = getNode(sourceModel_->index(last , 0, sourceModelFromParent)); // FIXME: handle unexpanded/unchecked nodes
            if(movingFirst && movingLast)
            {
                while(movingLast->status == TreeNode::Status::Expanded && !movingLast->children.empty())
                {
                    movingLast = movingLast->children.back().get();
                }
                auto bumpCount = movingLast->destIndex - movingFirst->destIndex + 1;
                if(fromIsUpper)
                {
                    bumpCount = -bumpCount;
                }
                // The nodes are splited into three parts as follows:
                //      +===+
                //     /|   |\
                //    / |   | \
                //   /  |   |  \
                //  / 1 | 2 | 3 \
                // +====+===+====+
                if(upperRange.mismatch != upperRange.range.end())
                {
                    // [1]
                    auto it = upperRange.mismatch; ++it;
                    auto node = divergingNode->children[*upperRange.mismatch].get();
                    while(it != upperRange.range.end())
                    {
                        bumpRowCountAfter(*(node->children[*it]), bumpCount);
                        node = node->children[*(it++)].get();
                    }
                    // [2]
                    for(auto it2 = divergingNode->children.begin() + (*upperRange.mismatch) + 1; it2 != divergingNode->children.begin() + (*lowerRange.mismatch); ++it2)
                    {
                        bumpRowCount(**it2, bumpCount);
                    }
                }
                // [3]
                {
                    auto it = lowerRange.mismatch; ++it;
                    auto node = divergingNode->children[*lowerRange.mismatch].get();
                    while(it != lowerRange.range.end())
                    {
                        bumpRowCountUntil(*(node->children[*it]), bumpCount, true);
                        node = node->children[*(it++)].get();
                    }
                }
            }
        }
        if(fromRange.uncheckedFirst == fromRange.range.end())
        {
            auto& fromChildren = fromRange.uncheckedParentNode->children;
            auto moveFirstNode = fromChildren[first].get();
            auto moveLastNode = fromChildren[last].get();
            if(toRange.uncheckedFirst == toRange.range.end())
            {
                auto& toChildren = toRange.uncheckedParentNode->children;
                auto moveBeforeDestNode = dest == 0? toRange.uncheckedParentNode: toRange.uncheckedParentNode->children[dest - 1].get();
                while(moveLastNode->status == TreeNode::Status::Expanded && !moveLastNode->children.empty())
                {
                    moveLastNode = moveLastNode->children.back().get();
                }
                beginMoveRows(QModelIndex(), moveFirstNode->destIndex, moveLastNode->destIndex, QModelIndex(), moveBeforeDestNode->destIndex + 1);
                std::move(
                    fromChildren.begin() + first,
                    fromChildren.begin() + last + 1,
                    std::inserter(toChildren, toChildren.begin() + dest)
                );
                auto bump = last - first + 1;
                for(auto& child: fromChildren | std::ranges::views::drop(last + 1))
                {
                    child->sourceModelIndex = sourceModel_->index(child->sourceModelIndex.row() - bump, 0, sourceModelFromParent);
                }
                for(auto& child: toChildren | std::ranges::views::drop(dest + bump))
                {
                    child->sourceModelIndex = sourceModel_->index(child->sourceModelIndex.row() + bump, 0, sourceModelToParent);
                }
                fromChildren.erase(
                    fromChildren.begin() + first,
                    fromChildren.begin() + last + 1
                );
                endMoveRows();
            }
            else
            {
                beginRemoveRows(QModelIndex(), moveFirstNode->destIndex, moveLastNode->destIndex);
                fromChildren.erase(
                    fromChildren.begin() + first,
                    fromChildren.begin() + last + 1
                );
                auto bump = last - first + 1;
                for(auto& child: fromChildren | std::ranges::views::drop(last + 1))
                {
                    child->sourceModelIndex = sourceModel_->index(child->sourceModelIndex.row() - bump, 0, sourceModelFromParent);
                }
                endRemoveRows();
            }
        }
        else
        {
            if(toRange.uncheckedFirst == toRange.range.end())
            {
                auto& toChildren = toRange.uncheckedParentNode->children;
                auto offset = (dest == 0?
                    toRange.uncheckedParentNode->destIndex:
                    toChildren[first - 1]->destIndex
                ) + 1;
                beginInsertRows(QModelIndex(), offset + 1, offset + (last - first + 1));
                std::generate_n(
                    std::inserter(toChildren, toChildren.begin() + dest),
                    last - first + 1,
                    [&sourceModelToParent, i = dest, offset, &toRange]() mutable
                    {
                        return std::make_unique<TreeNode>(
                            TreeNode {
                                .parent = toRange.uncheckedParentNode,
                                .sourceModelIndex = sourceModelToParent,
                                .destIndex = offset + i,
                                .status = TreeNode::Status::Unchecked
                            }
                        );
                    }
                );
                auto bump = last - first + 1;
                for(auto& child: toChildren | std::ranges::views::drop(dest + bump))
                {
                    child->sourceModelIndex = sourceModel_->index(child->sourceModelIndex.row() + bump, 0, sourceModelToParent);
                }
            }
        }
    }
}

void TreeModelToListModel::onSourceModelDataChanged(
    const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight,
    const QList<int>& roles)
{
    if(auto sourceParentIndex = sourceTopLeft.parent();
        sourceParentIndex == sourceBottomRight.parent())
    {
        if(auto node = getNode(sourceParentIndex); node->status == TreeNode::Status::Expanded)
        {
            dataChanged(
                index(node->children[sourceTopLeft.row()]->destIndex),
                index(node->children[sourceBottomRight.row()]->destIndex),
                {roles}
            );
        }
    }
}

void TreeModelToListModel::onSourceModelAboutToBeReset()
{
}

void TreeModelToListModel::onSourceModelReset()
{
}

const TreeModelToListModel::TreeNode*
TreeModelToListModel::getNode(const QModelIndex& sourceIndex, bool onlyIfExpanded) const
{
    if(sourceIndex == QModelIndex())
    {
        return nullptr;
    }
    std::vector<int> indices;
    indices.reserve(maxDepth_);
    for(auto index = sourceIndex; index != QModelIndex(); index = index.parent())
    {
        indices.emplace_back(index.row());
    }
    maxDepth_ = std::max(maxDepth_, static_cast<std::uint32_t>(indices.size()));
    const auto* ret = &root_;
    for(auto it = indices.rbegin(); it != indices.rend() && ((!onlyIfExpanded) || ret->status == TreeNode::Status::Expanded); ++it)
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

TreeModelToListModel::TreeNode* TreeModelToListModel::getNode(
    const QModelIndex& sourceIndex, bool onlyIfExpanded)
{
    return const_cast<TreeNode*>(
        static_cast<const TreeModelToListModel*>(this)->getNode(
            sourceIndex, onlyIfExpanded
        )
    );
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

std::pair<TreeModelToListModel::TreeNode*, QModelIndex> TreeModelToListModel::getNodeAndSourceIndex(int destIndex)
{
    auto ret = static_cast<const TreeModelToListModel*>(this)->getNodeAndSourceIndex(destIndex);
    return {const_cast<TreeNode*>(ret.first), ret.second};
}

void TreeModelToListModel::bumpRowCountAfter(TreeNode& node, int rowCount, bool inclusive)
{
    auto offset = node.sourceModelIndex.row() + 1;
    std::fprintf(stderr, "[DEBUG] `bumpRowCountAfter` node offset: %d\n", offset);
    for(auto& child: node.parent->children | std::views::drop(offset - inclusive))
    {
        child->destIndex += rowCount;
        bumpRowCount(*child, rowCount);
    }
}

void TreeModelToListModel::bumpRowCountUntil(TreeNode& node, int rowCount, bool inclusive)
{
    auto offset = node.sourceModelIndex.row();
    std::fprintf(stderr, "[DEBUG] `bumpRowCountAfter` node offset: %d\n", offset);
    for(auto& child: node.parent->children | std::views::take(offset + inclusive))
    {
        child->destIndex += rowCount;
        bumpRowCount(*child, rowCount);
    }
}

void TreeModelToListModel::bumpRowCount(TreeNode& node, int rowCount)
{
    for(auto& child: node.children)
    {
        child->destIndex += rowCount;
        bumpRowCount(*child, rowCount);
    }
}
}
