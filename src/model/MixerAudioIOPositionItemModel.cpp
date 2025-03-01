#include "MixerAudioIOPositionItemModel.hpp"

#include "model/MixerChannelInsertListModel.hpp"

// |  #  | node                 | ptr |  0  |  1  | row | hasChildren |
// | --- | :------------------- | --- | --- | --- | --- | ----------- |
// |  0  | `v channel-0       ` | N/A |     |     |  0  |      1      |
// |  1  | `|--v instrument   ` |  1  |     |     |  0  |      1      |
// |  2  | `|  |-- input 0    ` |  2  |  0  |     |  0  |      0      |
// |  3  | `|  v pre-fader    ` |  1  |     |     |  1  |      1      |
// |  4  | `|  |--v insert 0  ` |  3  |  1  |     |  0  |      1      |
// |  5  | `|  |  '-- input 0 ` |  4  |  1  |  0  |  0  |      0      |
// |  6  | `|  |--v insert 1  ` |  3  |  1  |     |  1  |      1      |
// |  7  | `|  |--v insert 2  ` |  3  |  1  |     |  2  |      1      |
// |  8  | `|  |--v insert 3  ` |  3  |  1  |     |  3  |      1      |
// |  9  | `|  v post-fader   ` |  1  |     |     |  2  |      1      |
// |  10 | `|  |--v insert 0  ` |  9  |  2  |     |  0  |      1      |
// |  11 | `|  |  '-- input 0 ` |  10 |  2  |  0  |  0  |      0      |
// |  12 | `|  |--v insert 1  ` |  9  |  2  |     |  1  |      1      |
// |  13 | `|  |--v insert 2  ` |  9  |  2  |     |  2  |      1      |
// |  14 | `|  '--v insert 3  ` |  9  |  2  |     |  3  |      1      |
// |  15 | `v channel-1       ` | N/A |     |     |  1  |      1      |
// |  16 | `v channel-2       ` | N/A |     |     |  2  |      1      |
// |  17 | `v channel-3       ` | N/A |     |     |  3  |      1      |
// |  18 | `v channel-4       ` | N/A |     |     |  4  |      1      |
// |  19 | `v channel-5       ` | N/A |     |     |  5  |      1      |
// |  20 | `v channel-6       ` | N/A |     |     |  6  |      1      |
// |  21 | `v channel-7       ` | N/A |     |     |  7  |      1      |

namespace YADAW::Model
{
MixerAudioIOPositionItemModel::MixerAudioIOPositionItemModel(
    MixerChannelListModel& mixerChannelListModel, bool isInput):
    mixerChannelListModel_(&mixerChannelListModel),
    isInput_(isInput)
{
    QObject::connect(
        mixerChannelListModel_, &MixerChannelListModel::rowsInserted,
        this, &MixerAudioIOPositionItemModel::onSourceModelRowsRemoved
    );
    QObject::connect(
        mixerChannelListModel_, &MixerChannelListModel::rowsAboutToBeRemoved,
        this, &MixerAudioIOPositionItemModel::onSourceModelRowsAboutToBeRemoved
    );
    QObject::connect(
        mixerChannelListModel_, &MixerChannelListModel::rowsRemoved,
        this, &MixerAudioIOPositionItemModel::onSourceModelRowsRemoved
    );
    QObject::connect(
        mixerChannelListModel_, &MixerChannelListModel::dataChanged,
        this, &MixerAudioIOPositionItemModel::onSourceModelDataChanged
    );
}

// Check field from `parent`: `internalPointer()`
QModelIndex MixerAudioIOPositionItemModel::index(
    int row, int column, const QModelIndex& parent) const
{
    const auto& vec = parent == QModelIndex()?
        nodeDataTree_:
        static_cast<NodeData*>(parent.internalPointer())->children;
    if(row >= 0 && row < nodeDataTree_.size())
    {
        return createIndex(row, column, nodeDataTree_[row].get());
    }
    return QModelIndex();
}

//  Check field from `child`: `internalPointer()`
QModelIndex MixerAudioIOPositionItemModel::parent(const QModelIndex& child) const
{
    if(auto ptr = child.internalPointer())
    {
        return createIndex(
            0, 0, static_cast<NodeData*>(ptr)->parent
        );
    }
    return QModelIndex();
}

// Check child from `parent`: `internalPointer()`, `row()`
bool MixerAudioIOPositionItemModel::hasChildren(const QModelIndex& parent) const
{
    auto nodeData = static_cast<NodeData*>(parent.internalPointer());
    return nodeData == nullptr || nodeData->indent < 2;
}

// Check child from `parent`: `internalPointer()`, `row()`
int MixerAudioIOPositionItemModel::rowCount(const QModelIndex& parent) const
{
    if(parent == QModelIndex())
    {
        return nodeDataTree_.size();
    }
    auto nodeData = static_cast<NodeData*>(parent.internalPointer());
    return nodeData->children[parent.row()]->children.size();
}

QVariant MixerAudioIOPositionItemModel::data(const QModelIndex& index, int role) const
{
    if(index == QModelIndex())
    {
        if(auto row = index.row(); row >= 0 && row < nodeDataTree_.size())
        {
            switch(role)
            {
            case Role::IsTreeNode:
                return QVariant::fromValue<bool>(true);
            case Role::TreeName:
                return QVariant::fromValue<QString>(
                    mixerChannelListModel_->data(
                        mixerChannelListModel_->index(row),
                        MixerChannelListModel::Role::NameWithIndex
                    ).value<QString>()
                );
            }
        }
    }
    // auto nodeData = static_cast<NodeData*>(index.internalPointer());
    // if(auto row = index.row(); row >= 0 && row < nodeData->children.size())
    // {
    //     switch(role)
    //     {
    //     case Role::IsTreeNode:
    //     {
    //         if(nodeData->parent)
    //         {
    //             if(row == 0 && mixerChannelListModel_->data(
    //                 mixerChannelListModel_->index()))
    //         }
    //         if(nodeData->indent == 1 && row == 0 && )
    //         {
    //             //
    //         }
    //     }
    //     case Role::TreeName:
    //     {
    //         //
    //     }
    //     case Role::Position:
    //     {
    //         //
    //     }
    //     case Role::ChannelConfig:
    //     {
    //         //
    //     }
    //     case Role::ChannelCount:
    //     {
    //         //
    //     }
    //     }
    // }
    return {};
}

bool MixerAudioIOPositionItemModel::isComparable(int roleIndex) const
{
    return roleIndex == Role::TreeName;
}

bool MixerAudioIOPositionItemModel::isFilterable(int roleIndex) const
{
    return roleIndex == Role::ChannelConfig
        || roleIndex == Role::ChannelCount;
}

bool MixerAudioIOPositionItemModel::isSearchable(int roleIndex) const
{
    return roleIndex == Role::TreeName;
}

QList<int> MixerAudioIOPositionItemModel::treeNodeRoles() const
{
    return {Role::TreeName};
}

QList<int> MixerAudioIOPositionItemModel::leafNodeRoles() const
{
    return {Role::Position, Role::ChannelConfig, Role::ChannelCount};
}

bool MixerAudioIOPositionItemModel::isTreeNode(const QModelIndex& index) const
{
    return data(index, Role::IsTreeNode).value<bool>();
}

bool MixerAudioIOPositionItemModel::isLess(
    int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(roleIndex == Role::TreeName)
    {
        return data(lhs, roleIndex).value<QString>() < data(rhs, roleIndex).value<QString>();
    }
    return false;
}

bool MixerAudioIOPositionItemModel::isSearchPassed(
    int roleIndex, const QModelIndex& modelIndex, const QString& string,
    Qt::CaseSensitivity caseSensitivity) const
{
    if(roleIndex == Role::TreeName)
    {
        return data(modelIndex, roleIndex).value<QString>().contains(
            string, caseSensitivity
        );
    }
    return false;
}

bool MixerAudioIOPositionItemModel::isPassed(const QModelIndex& modelIndex,
    int role, const QVariant& variant) const
{
    if(role == Role::ChannelConfig)
    {
        return data(modelIndex, role).value<int>() == variant.value<int>();
    }
    else if(role == Role::ChannelCount)
    {
        return data(modelIndex, role).value<int>() == variant.value<int>();
    }
    return false;
}

QModelIndex MixerAudioIOPositionItemModel::findIndexByID(const QString& id) const
{
    return {};
}

std::unique_ptr<MixerAudioIOPositionItemModel::NodeData>
MixerAudioIOPositionItemModel::createRootNodeData(
    std::uint32_t index)
{
    auto ret = std::make_unique<NodeData>(
        NodeData {
            .indent = NodeData::Indent::ChannelIndex,
            .index = index,
            .parent = nullptr
        }
    );
    ret->children.reserve(3);
    std::uint32_t i = 0;
    // instrument
    if(
        mixerChannelListModel_->data(
            mixerChannelListModel_->index(index),
            MixerChannelListModel::Role::InstrumentExist
        ).value<bool>()
    )
    {
        ret->children.emplace_back(
            std::make_unique<NodeData>(
                NodeData {
                    .indent = NodeData::Indent::InChannelPosition,
                    .index = i++,
                    .parent = ret.get()
                }
            )
        );
    }
    auto& preFaderInsertNode = ret->children.emplace_back(
        std::make_unique<NodeData>(
            NodeData {
                .indent = NodeData::Indent::InChannelPosition,
                .index = i++,
                .parent = ret.get()
            }
        )
    );
    auto preFaderInsertList = static_cast<MixerChannelInsertListModel*>(
        mixerChannelListModel_->data(
            mixerChannelListModel_->index(index),
            MixerChannelListModel::Role::Inserts
        ).value<QObject*>()
    );
    preFaderInsertNode->children.reserve(preFaderInsertList->itemCount());
    std::generate_n(
        std::back_inserter(preFaderInsertNode->children),
        preFaderInsertList->itemCount(),
        [parent = preFaderInsertNode.get(), index = 0U]() mutable
        {
            return std::make_unique<NodeData>(
                NodeData {
                    .indent = NodeData::Indent::InsertIndex,
                    .index = index++,
                    .parent = parent
                }
            );
        }
    );
    auto& postFaderInserts = ret->children.emplace_back(
        std::make_unique<NodeData>(
            NodeData {
                .indent = NodeData::Indent::InChannelPosition,
                .index = i++,
                .parent = ret.get()
            }
        )
    );
    // TODO: Not implemented
    return ret;
}

void MixerAudioIOPositionItemModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
    std::generate_n(
        std::inserter(nodeDataTree_, nodeDataTree_.begin() + first), last - first + 1,
        [this, currentIndex = std::uint32_t(first)]() mutable
        {
            return createRootNodeData(currentIndex++);
        }
    );
    FOR_RANGE(i, last + 1, nodeDataTree_.size())
    {
        nodeDataTree_[i]->index = i;
    }
    endInsertRows();
}

void MixerAudioIOPositionItemModel::onSourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void MixerAudioIOPositionItemModel::onSourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    nodeDataTree_.erase(
        nodeDataTree_.begin() + first,
        nodeDataTree_.begin() + last + 1
    );
    FOR_RANGE(i, first, nodeDataTree_.size())
    {
        nodeDataTree_[i]->index = i;
    }
    endRemoveRows();
}

void MixerAudioIOPositionItemModel::onSourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    //
}

void MixerAudioIOPositionItemModel::onInsertModelRowsInserted(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelRowsAboutToBeRemoved(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelRowsRemoved(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelDataChanged(
    std::uint32_t index, const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{}
}
