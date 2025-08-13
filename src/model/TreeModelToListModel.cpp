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
    }
    // TODO
    sourceModelChanged();
}

QModelIndex TreeModelToListModel::sourceToDest(const QModelIndex& source) const
{
    return {};
}

QModelIndex TreeModelToListModel::destToSource(const QModelIndex& dest) const
{
    if(auto index = dest.row(); index >= 0)
    {
        QModelIndex sourceParent;
        auto* children = &root_.children;
        while(!children->empty())
        {
            auto lowerBound = std::lower_bound(
                children->begin(), children->end(), index,
                &TreeModelToListModel::compareTreeNodeIndex
            );
            if(lowerBound == children->end())
            {
                assert(false);
                return {};
            }
            else if(lowerBound->get()->destIndex == index)
            {
                return sourceModel_->index(
                    lowerBound - children->begin(), 0, sourceParent
                );
            }
            else if(lowerBound == children->begin())
            {
                assert(false);
                return {};
            }
            else
            {
                --lowerBound;
                sourceParent = sourceModel_->index(
                    lowerBound - children->begin(), 0, sourceParent
                );
                children = &(lowerBound->get()->children);
            }
        }
    }
    return {};
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
    if(role >= Role::Depth && role < Qt::UserRole)
    {
        return {}; // TODO
    }
    if(auto sourceIndex = destToSource(index); sourceIndex.isValid())
    {
        return sourceModel_->data(sourceIndex, role);
    }
    return {};
}

bool TreeModelToListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role >= Role::Depth && role < Qt::UserRole)
    {
        return false; // TODO
    }
    if(auto sourceIndex = destToSource(index); sourceIndex.isValid())
    {
        return sourceModel_->setData(sourceIndex, value, role);
    }
    return false;
}

RoleNames TreeModelToListModel::roleNames() const
{
    auto ret = sourceModel_? sourceModel_->roleNames(): RoleNames();
    ret.insert(Role::Depth,       "tmtlm_depth");
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

bool TreeModelToListModel::compareTreeNodeIndex(
    const std::unique_ptr<TreeModelToListModel::TreeNode>& node, int index)
{
    return node->destIndex < index;
}
}
