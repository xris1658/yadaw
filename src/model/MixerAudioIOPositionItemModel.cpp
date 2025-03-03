#include "MixerAudioIOPositionItemModel.hpp"

#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/MixerChannelInsertListModel.hpp"

#include <QCoreApplication>

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
        MixerChannelListModel& mixerAudioInputChannelListModel,
        MixerChannelListModel& mixerRegularChannelListModel,
        MixerChannelListModel& mixerAudioOutputChannelListModel,
        bool isInput):
    mixerChannelListModels_{
        &mixerAudioInputChannelListModel,
        &mixerRegularChannelListModel,
        &mixerAudioOutputChannelListModel
    },
    isInput_(isInput)
{
    rootNode_.children.reserve(3);
    rootNode_.children.emplace_back(
        createNode(MixerChannelListModel::ListType::AudioHardwareInput)
    );
    rootNode_.children.emplace_back(
        createNode(MixerChannelListModel::ListType::Regular)
    );
    rootNode_.children.emplace_back(
        createNode(MixerChannelListModel::ListType::AudioHardwareOutput)
    );
}

QModelIndex MixerAudioIOPositionItemModel::index(
    int row, int column, const QModelIndex& parent) const
{
    auto nodeData = parent == QModelIndex()? &rootNode_: getNodeData(parent);
    return createIndex(row, column, nodeData);
}

QModelIndex MixerAudioIOPositionItemModel::parent(const QModelIndex& child) const
{
    auto nodeData = getNodeData(child);
    if(auto parentPtr = nodeData->parent)
    {
        for(auto it = parentPtr->children.begin(); it != parentPtr->children.end(); ++it)
        {
            if(it->get() == nodeData)
            {
                return createIndex(
                    it - parentPtr->children.begin(), 0, parentPtr
                );
            }
        }
    }
    return {};
}

bool MixerAudioIOPositionItemModel::hasChildren(const QModelIndex& parent) const
{
    return parent == QModelIndex()
        || !getNodeData(parent)->children[parent.row()]->children.empty();
}

int MixerAudioIOPositionItemModel::rowCount(const QModelIndex& parent) const
{
    auto nodeData = parent == QModelIndex()? &rootNode_: getNodeData(parent);
    return nodeData->children.size();
}

QVariant MixerAudioIOPositionItemModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0)
    {
        if(index.parent() == QModelIndex())
        {
            switch(role)
            {
            case Role::TreeName:
                return QVariant::fromValue(
                    row == 0? QCoreApplication::translate("MixerAudioIOPositionItemModel", "Audio Input Bus" ):
                    row == 1? QCoreApplication::translate("MixerAudioIOPositionItemModel", "Regular Channel" ):
                    row == 2? QCoreApplication::translate("MixerAudioIOPositionItemModel", "Audio Output Bus"):
                    QString()
                );
            case Role::IsTreeNode:
                return QVariant::fromValue(true);
            }
        }
        auto nodeData = getNodeData(index);
        std::uint32_t indices[NodeData::Indent::Count];
        for(auto node = nodeData; node != nullptr; node = node->parent)
        {
            indices[node->indent] = node->index;
        }
        switch(nodeData->indent + 1)
        {
        case NodeData::Indent::ChannelIndex:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            if(role == Role::TreeName)
            {
                return model->data(
                    model->index(row), MixerChannelListModel::Role::NameWithIndex
                );
            }
            else if(role == Role::IsTreeNode)
            {
                return true;
            }
        }
        case NodeData::Indent::InChannelPosition:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            auto inChannelPosition = nodeData->children[row]->index;
            if(inChannelPosition == NodeData::NodeInChannelPosition::Instrument)
            {
                if(role == Role::TreeName)
                {
                    return QVariant::fromValue(
                        QCoreApplication::translate(
                            "MixerAudioIOPositionItemModel",
                            "Instrument: %1"
                        ).arg(
                            model->data(
                                model->index(indices[NodeData::Indent::ChannelIndex]),
                                MixerChannelListModel::Role::InstrumentName
                            ).value<QString>()
                        )
                    );
                }
                else if(role == Role::IsTreeNode)
                {
                    return true;
                }
            }
            else if(inChannelPosition == NodeData::NodeInChannelPosition::PreFaderInserts)
            {
                if(role == Role::TreeName)
                {
                    return QVariant::fromValue(
                        QCoreApplication::translate(
                            "MixerAudioIOPositionItemModel",
                            "Pre-Fader Inserts"
                        )
                    );
                }
                else if(role == Role::IsTreeNode)
                {
                    return true;
                }
            }
            else if(inChannelPosition == NodeData::NodeInChannelPosition::PostFaderInserts)
            {
                if(role == Role::TreeName)
                {
                    return QVariant::fromValue(
                        QCoreApplication::translate(
                            "MixerAudioIOPositionItemModel",
                            "Post-Fader Inserts"
                        )
                    );
                }
                else if(role == Role::IsTreeNode)
                {
                    return true;
                }
            }
        }
        case NodeData::Indent::InsertIndex:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            if(nodeData->index == NodeData::NodeInChannelPosition::Instrument)
            {
                // TODO: Instrument aux I/O
            }
            if(nodeData->index == NodeData::NodeInChannelPosition::PreFaderInserts)
            {
                auto insertModel = static_cast<MixerChannelInsertListModel*>(
                    model->data(
                        model->index(row), MixerChannelListModel::Role::Inserts
                    ).value<QObject*>()
                );
                if(row < insertModel->itemCount())
                {
                    if(role == Role::TreeName)
                    {
                        return QCoreApplication::translate(
                            "MixerAudioIOPositionItemModel",
                            "%1: %2",
                            "Insert Index: Insert Name"
                        ).arg(
                            QString::number(row),
                            insertModel->data(
                                insertModel->index(row),
                                MixerChannelInsertListModel::Role::Name
                            ).value<QString>()
                        );
                    }
                    else if(role == Role::IsTreeNode)
                    {
                        return true;
                    }
                }
            }
        }
        case NodeData::Indent::Count:
        {
            // TODO: Insert aux I/O
        }
        }
    }
    return {};
}

bool MixerAudioIOPositionItemModel::isComparable(int roleIndex) const
{
    return false;
}

bool MixerAudioIOPositionItemModel::isFilterable(int roleIndex) const
{
    return false;
}

bool MixerAudioIOPositionItemModel::isSearchable(int roleIndex) const
{
    return false;
}

QList<int> MixerAudioIOPositionItemModel::treeNodeRoles() const
{
    return {};
}

QList<int> MixerAudioIOPositionItemModel::leafNodeRoles() const
{
    return {};
}

bool MixerAudioIOPositionItemModel::isTreeNode(const QModelIndex& index) const
{
    return false;
}

bool MixerAudioIOPositionItemModel::isLess(
    int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}

bool MixerAudioIOPositionItemModel::isSearchPassed(
    int roleIndex, const QModelIndex& modelIndex, const QString& string,
    Qt::CaseSensitivity caseSensitivity) const
{
    return false;
}

bool MixerAudioIOPositionItemModel::isPassed(const QModelIndex& modelIndex,
    int role, const QVariant& variant) const
{
    return false;
}

QModelIndex MixerAudioIOPositionItemModel::findIndexByID(const QString& id) const
{
    return QModelIndex();
}

std::unique_ptr<MixerAudioIOPositionItemModel::NodeData> MixerAudioIOPositionItemModel::createNode(
    MixerChannelListModel::ListType type)
{
    auto ret = std::make_unique<NodeData>(
        NodeData {
            .indent = NodeData::Indent::ListType,
            .index  = type,
            .parent = rootNode_.children[type].get()
        }
    );
    ret->children.reserve(mixerChannelListModels_[type]->itemCount());
    FOR_RANGE0(i, mixerChannelListModels_[type]->itemCount())
    {
        auto& channelNode = ret->children.emplace_back(
            std::make_unique<NodeData>(
                NodeData {
                    .indent = NodeData::Indent::ChannelIndex,
                    .index  = i,
                    .parent = ret.get()
                }
            )
        );
        bool hasInstrument = mixerChannelListModels_[type]->data(
            mixerChannelListModels_[type]->index(i),
            MixerChannelListModel::Role::InstrumentExist
        ).value<bool>();
        channelNode->children.reserve(
            2 + hasInstrument
        );
        if(hasInstrument)
        {
            auto& instrumentNode = channelNode->children.emplace_back(
                std::make_unique<NodeData>(
                    NodeData {
                        .indent = NodeData::Indent::InChannelPosition,
                        .index  = NodeData::NodeInChannelPosition::Instrument,
                        .parent = channelNode.get()
                    }
                )
            );
            if(isInput_)
            {
                auto instrumentAudioInputs = static_cast<AudioDeviceIOGroupListModel*>(
                    mixerChannelListModels_[type]->data(
                        mixerChannelListModels_[type]->index(i),
                        MixerChannelListModel::Role::InstrumentAudioInputs
                    ).value<QObject*>()
                );
                auto count = instrumentAudioInputs->itemCount();
                instrumentNode->children.reserve(count);
                FOR_RANGE0(j, count)
                {
                    instrumentNode->children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelGroupIndex,
                                .index = j,
                                .parent = instrumentNode.get()
                            }
                        )
                    );
                }
            }
            else
            {
                auto instrumentAudioOutputs = static_cast<AudioDeviceIOGroupListModel*>(
                    mixerChannelListModels_[type]->data(
                        mixerChannelListModels_[type]->index(i),
                        MixerChannelListModel::Role::InstrumentAudioOutputs
                    )
                );
                auto count = instrumentAudioOutputs->itemCount();
                instrumentNode->children.reserve(count - 1);
                auto insertModel = static_cast<MixerChannelInsertListModel*>(
                    mixerChannelListModels_[type]->data(
                        mixerChannelListModels_[type]->index(i),
                        MixerChannelListModel::Role::Inserts
                    )
                );
                auto mainOutput = insertModel->inserts().inChannelGroupIndex();
                FOR_RANGE0(j, mainOutput)
                {
                    instrumentNode->children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelGroupIndex,
                                .index  = j,
                                .parent = instrumentNode.get()
                            }
                        )
                    );
                }
                FOR_RANGE(j, mainOutput + 1, count)
                {
                    instrumentNode->children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelGroupIndex,
                                .index  = j,
                                .parent = instrumentNode.get()
                            }
                        )
                    );
                }
            }
        }
        auto& preFaderInsertNode = channelNode->children.emplace_back(
            std::make_unique<NodeData>(
                NodeData {
                    .indent = NodeData::Indent::InChannelPosition,
                    .index  = NodeData::NodeInChannelPosition::PreFaderInserts,
                    .parent = channelNode.get()
                }
            )
        );
        const auto* insertListModel = static_cast<const MixerChannelInsertListModel*>(
            mixerChannelListModels_[type]->data(
                mixerChannelListModels_[type]->index(i),
                MixerChannelListModel::Role::Inserts
            ).value<QObject*>()
        );
        auto insertCount = insertListModel->itemCount();
        preFaderInsertNode->children.reserve(insertCount);
        FOR_RANGE0(j, insertCount)
        {
            preFaderInsertNode->children.emplace_back(
                std::make_unique<NodeData>(
                    NodeData {
                        .indent = NodeData::Indent::InsertIndex,
                        .index  = j,
                        .parent = preFaderInsertNode.get()
                    }
                )
            );
            if(isInput_)
            {
                auto list = static_cast<AudioDeviceIOGroupListModel*>(
                    insertListModel->data(
                        insertListModel->index(j),
                        isInput_?
                            MixerChannelInsertListModel::Role::AudioInputs:
                            MixerChannelInsertListModel::Role::AudioOutputs
                    ).value<QObject*>()
                );
                std::uint32_t firstMain = list->itemCount();
                FOR_RANGE0(k, list->itemCount())
                {
                    if(
                        list->data(
                            list->index(k),
                            AudioDeviceIOGroupListModel::Role::IsMain
                        ).value<bool>()
                    )
                    {
                        firstMain = k;
                        break;
                    }
                }
                preFaderInsertNode->children.reserve(list->itemCount() - 1);
                FOR_RANGE0(k, firstMain)
                {
                    preFaderInsertNode->children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelGroupIndex,
                                .index  = k,
                                .parent = preFaderInsertNode.get()
                            }
                        )
                    );
                }
                FOR_RANGE(k, firstMain + 1, list->itemCount())
                {
                    preFaderInsertNode->children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelGroupIndex,
                                .index  = k,
                                .parent = preFaderInsertNode.get()
                            }
                        )
                    );
                }
            }
        }
        // TODO: Implement post-fader inserts
    }
    return ret;
}

const MixerAudioIOPositionItemModel::NodeData*
MixerAudioIOPositionItemModel::getNodeData(const QModelIndex& index) const
{
    return static_cast<const NodeData*>(index.internalPointer());
}

MixerAudioIOPositionItemModel::NodeData* MixerAudioIOPositionItemModel::getNodeData(const QModelIndex& index)
{
    return static_cast<NodeData*>(index.internalPointer());
}

void MixerAudioIOPositionItemModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{}

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
