#include "model/TreeModelToListModel.hpp"

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
    if(sourceModel_)
    {
        QObject::disconnect(sourceModel_, nullptr, this, nullptr);
        beginResetModel();
        root_.children.clear();
        // TODO
        endResetModel();
    }
    sourceModel_ = sourceModel;
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
    // TODO
    sourceModelChanged();
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
}
