#include "MixerAudioIOPositionItemModel.hpp"

#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "util/QmlUtil.hpp"

#include <QCoreApplication>

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
    YADAW::Util::setCppOwnership(*this);
    Indices indices;
    initChildren(rootNode_, indices);
    FOR_RANGE0(i, 3)
    {
        auto parentIndex = MixerAudioIOPositionItemModel::index(i, 0, QModelIndex());
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::rowsInserted,
            [this, parentIndex, sender = mixerChannelListModels_[i], &treeNode = rootNode_.children[i]]
            (const QModelIndex&, int first, int last) // MixerChannelListModel::rowsInserted
            {
                beginInsertRows(parentIndex, first, last);
                std::generate_n(
                    std::inserter(treeNode->children, treeNode->children.begin() + first), last - first + 1,
                    [&treeNode, channelIndex = static_cast<std::uint32_t>(first)]() mutable
                    {
                        return std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelIndex,
                                .index  = channelIndex++,
                                .parent = treeNode.get()
                            }
                        );
                    }
                );
                endInsertRows();
                Indices indices;
                for(auto node = treeNode.get(); node != &rootNode_; node = node->parent)
                {
                    indices[node->indent] = node->index;
                }
                FOR_RANGE(j, first, last + 1)
                {
                    initChildren(*treeNode->children[j], indices);
                    auto mixerChannelInsertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
                        sender->data(
                            sender->index(j), MixerChannelListModel::Role::Inserts
                        ).value<QObject*>()
                    );
                    QObject::connect(
                        mixerChannelInsertListModel,
                        &MixerChannelInsertListModel::channelIndexChanged,
                        [mixerChannelInsertListModel, node = treeNode->children[j].get()]()
                        {
                            node->index = mixerChannelInsertListModel->channelIndex();
                        }
                    );
                    QObject::connect(
                        mixerChannelInsertListModel,
                        &MixerChannelInsertListModel::rowsInserted,
                        [this, parentIndex, mixerChannelListModel = sender, mixerChannelInsertListModel]
                        (const QModelIndex& parent, int first, int last)
                        {
                            auto destParentIndex = MixerAudioIOPositionItemModel::index(
                                mixerChannelListModel->data(
                                    mixerChannelListModel->index(
                                        mixerChannelInsertListModel->channelIndex()
                                    ),
                                    MixerChannelListModel::Role::InstrumentExist
                                ).value<bool>()? 1: 0,
                                0,
                                MixerAudioIOPositionItemModel::index(
                                    mixerChannelInsertListModel->channelIndex(), 0,
                                    parentIndex
                                )
                            );
                            auto destParentNode = getNodeData(destParentIndex);
                            beginInsertRows(destParentIndex, first, last);
                            std::generate_n(
                                std::inserter(
                                    destParentNode->children,
                                    destParentNode->children.begin() + first
                                ),
                                last - first + 1,
                                [destParentNode, channelIndex = static_cast<std::uint32_t>(first)]() mutable
                                {
                                    return std::make_unique<NodeData>(
                                        NodeData {
                                            .indent = NodeData::Indent::InsertIndex,
                                            .index  = channelIndex++,
                                            .parent = destParentNode
                                        }
                                    );
                                }
                            );
                            endInsertRows();
                            FOR_RANGE(j, last + 1, destParentNode->children.size())
                            {
                                destParentNode->children[j]->index = j;
                                updatePositionCompleteNames(*destParentNode->children[j]);
                            }
                            Indices indices;
                            for(auto node = destParentNode; node != &rootNode_; node = node->parent)
                            {
                                indices[node->indent] = node->index;
                            }
                            FOR_RANGE(j, first, last + 1)
                            {
                                auto& children = *destParentNode->children[j];
                                indices[children.indent] = children.index;
                                initChildren(children, indices);
                            }
                        }
                    );
                    QObject::connect(
                        mixerChannelInsertListModel,
                        &MixerChannelInsertListModel::rowsAboutToBeRemoved,
                        [this, parentIndex, mixerChannelListModel = sender, mixerChannelInsertListModel]
                        (const QModelIndex&, int first, int last)
                        {
                            auto destParentIndex = MixerAudioIOPositionItemModel::index(
                                mixerChannelListModel->data(
                                    mixerChannelListModel->index(mixerChannelInsertListModel->channelIndex()),
                                    MixerChannelListModel::Role::InstrumentExist
                                ).value<bool>()? 1: 0,
                                0,
                                MixerAudioIOPositionItemModel::index(
                                    mixerChannelInsertListModel->channelIndex(), 0,
                                    parentIndex
                                )
                            );
                            beginRemoveRows(destParentIndex, first, last);
                            auto destParentNode = getNodeData(destParentIndex);
                            destParentNode->children.erase(
                                destParentNode->children.begin() + first,
                                destParentNode->children.begin() + last + 1
                            );
                            FOR_RANGE(j, first, destParentNode->children.size())
                            {
                                destParentNode->children[j]->index = j;
                                updatePositionCompleteNames(*destParentNode->children[j]);
                            }
                            endRemoveRows();
                        }
                    );
                }
                FOR_RANGE(j, last + 1, treeNode->children.size())
                {
                    updatePositionCompleteNames(*(treeNode->children[j]));
                }
            }
        );
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::rowsAboutToBeRemoved,
            [this, i](const QModelIndex& parent, int first, int last)
            {
                beginRemoveRows(
                    MixerAudioIOPositionItemModel::index(i, 0, QModelIndex()),
                    first, last
                );
            }
        );
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::rowsRemoved,
            [this, parentIndex, channelList = mixerChannelListModels_[i], &treeNode = rootNode_.children[i]]
            (const QModelIndex& parent, int first, int last) // MixerChannelListModel::rowsRemoved
            {
                treeNode->children.erase(
                    treeNode->children.begin() + first,
                    treeNode->children.begin() + last + 1
                );
                FOR_RANGE(j, first, treeNode->children.size())
                {
                    updatePositionCompleteNames(*treeNode->children[j]);
                }
                endRemoveRows();
            }
        );
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::dataChanged,
            [this, i](const QModelIndex& topLeft,
                const QModelIndex& bottomRight, const QList<int>& roles)
            {
                for(auto role: roles)
                {
                    if(role == MixerChannelListModel::Role::NameWithIndex)
                    {
                        FOR_RANGE(j, topLeft.row(), bottomRight.row() + 1)
                        {
                            auto index = MixerAudioIOPositionItemModel::index(
                                j, 0,
                                MixerAudioIOPositionItemModel::index(
                                    i, 0, QModelIndex()
                                )
                            );
                            dataChanged(
                                index, index,
                                {MixerAudioIOPositionItemModel::Role::TreeName}
                            );
                        }
                    }
                }
            }
        );
    }
    QObject::connect(
        &mixerRegularChannelListModel,
        &MixerChannelListModel::dataChanged,
        [this, sender = &mixerRegularChannelListModel](
            const QModelIndex& topLeft, const QModelIndex& bottomRight,
            const QList<int>& roles)
        {
            for(auto role: roles)
            {
                if(role == MixerChannelListModel::Role::InstrumentExist)
                {
                    auto parentIndex = MixerAudioIOPositionItemModel::index(
                        1, 0, QModelIndex()
                    );
                    FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
                    {
                        auto index = MixerAudioIOPositionItemModel::index(i, 0, parentIndex);
                        auto& parentNode = rootNode_.children[
                            YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
                        ]->children[i];
                        auto instrumentExists = sender->data(
                            sender->index(i), MixerChannelListModel::Role::InstrumentExist
                        ).value<bool>();
                        auto oldRowCount = MixerAudioIOPositionItemModel::rowCount(
                            MixerAudioIOPositionItemModel::index(i, 0, parentIndex)
                        );
                        if(instrumentExists && oldRowCount == 1)
                        {
                            beginInsertRows(index, 0, 0);
                            auto instrumentNode = std::make_unique<NodeData>(
                                NodeData {
                                    .indent = NodeData::Indent::InChannelPosition,
                                    .index  = NodeData::NodeInChannelPosition::Instrument,
                                    .parent = parentNode.get()
                                }
                            );
                            Indices indices;
                            for(auto node = instrumentNode.get(); node != &rootNode_; node = node->parent)
                            {
                                indices[node->indent] = node->index;
                            }
                            initChildren(*instrumentNode, indices);
                            parentNode->children.emplace(
                                parentNode->children.begin(),
                                std::move(instrumentNode)
                            );
                            endInsertRows();
                        }
                        else if(!instrumentExists && oldRowCount == 2)
                        {
                            beginRemoveRows(index, 0, 0);
                            parentNode->children.erase(
                                parentNode->children.begin()
                            );
                            endRemoveRows();
                        }
                    }
                }
            }
        }
    );
    QObject::connect(
        &mixerRegularChannelListModel,
        &MixerChannelListModel::instrumentAboutToBeRemoved,
        [this, sender = &mixerRegularChannelListModel](std::uint32_t index)
        {
            auto node = rootNode_.children[YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList]
                ->children[index]
                ->children[NodeData::NodeInChannelPosition::Instrument].get();
            node->positions.clear();
        }
    );
}

QModelIndex MixerAudioIOPositionItemModel::index(
    int row, int column, const QModelIndex& parent) const
{
    if(hasIndex(row, column, parent))
    {
        return createIndex(row, column, getNodeData(parent));
    }
    return QModelIndex();
}

QModelIndex MixerAudioIOPositionItemModel::parent(const QModelIndex& child) const
{
    auto nodeData = getParentNodeData(child);
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
    const auto node = getNodeData(parent);
    return node->indent != NodeData::Indent::ChannelGroupIndex;
}

int MixerAudioIOPositionItemModel::rowCount(const QModelIndex& parent) const
{
    return getNodeData(parent)->children.size();
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
        for(auto node = nodeData; node != &rootNode_; node = node->parent)
        {
            indices[node->indent] = node->index;
        }
        switch(nodeData->indent)
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
            break;
        }
        case NodeData::Indent::InChannelPosition:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            auto inChannelPosition = indices[NodeData::Indent::InChannelPosition];
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
            break;
        }
        case NodeData::Indent::InsertIndex:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            auto* insertModel = static_cast<MixerChannelInsertListModel*>(
                model->data(
                    model->index(indices[NodeData::Indent::ChannelIndex]),
                    MixerChannelListModel::Role::Inserts
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
                        QString::number(row + 1),
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
            break;
        }
        case NodeData::Indent::ChannelGroupIndex:
        {
            auto* model = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
            auto inChannelPosition = indices[NodeData::Indent::InChannelPosition];
            AudioDeviceIOGroupListModel* ioList = nullptr;
            if(inChannelPosition == NodeData::NodeInChannelPosition::Instrument)
            {
                ioList = static_cast<AudioDeviceIOGroupListModel*>(
                    model->data(
                        model->index(indices[NodeData::Indent::ChannelIndex]),
                        isInput_?
                            MixerChannelListModel::Role::InstrumentAudioInputs:
                            MixerChannelListModel::Role::InstrumentAudioOutputs
                    ).value<QObject*>()
                );
            }
            else if(inChannelPosition == NodeData::NodeInChannelPosition::PreFaderInserts)
            {
                auto insertModel = static_cast<MixerChannelInsertListModel*>(
                    model->data(
                        model->index(indices[NodeData::Indent::ChannelIndex]),
                        MixerChannelListModel::Role::Inserts
                    ).value<QObject*>()
                );
                ioList = static_cast<AudioDeviceIOGroupListModel*>(
                    insertModel->data(
                        insertModel->index(indices[NodeData::Indent::InsertIndex]),
                        isInput_?
                            MixerChannelInsertListModel::Role::AudioInputs:
                            MixerChannelInsertListModel::Role::AudioOutputs
                    ).value<QObject*>()
                );
            }
            switch(role)
            {
            case Role::IsTreeNode:
                return false;
            case Role::TreeName:
                return ioList->data(
                    ioList->index(indices[NodeData::Indent::ChannelGroupIndex]),
                    AudioDeviceIOGroupListModel::Role::Name
                );
            case Role::ChannelConfig:
                return ioList->data(
                    ioList->index(indices[NodeData::Indent::ChannelGroupIndex]),
                    AudioDeviceIOGroupListModel::Role::ChannelConfig
                );
            case Role::ChannelCount:
                return ioList->data(
                    ioList->index(indices[NodeData::Indent::ChannelGroupIndex]),
                    AudioDeviceIOGroupListModel::Role::ChannelCount
                );
            case Role::Position:
                return QVariant::fromValue<QObject*>(
                    getNodeData(index.parent())->positions[index.row()].get()
                );
            }
            break;
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

std::optional<YADAW::Audio::Mixer::Mixer::Position>
MixerAudioIOPositionItemModel::getPosition(const QModelIndex& index) const
{
    if(index.isValid())
    {
        auto nodeData = getNodeData(index);
        if(nodeData->indent == NodeData::Indent::ChannelGroupIndex)
        {
            Indices indices;
            for(auto node = nodeData; node != &rootNode_; node = node->parent)
            {
                indices[node->indent] = node->index;
            }
            if(auto position = indices[NodeData::Indent::InChannelPosition];
                position == NodeData::NodeInChannelPosition::Instrument)
            {
                const auto& mixer = mixerChannelListModels_[YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList]->mixer();
                if(isInput_)
                {
                    return {
                        YADAW::Audio::Mixer::Mixer::Position {
                            .type = YADAW::Audio::Mixer::Mixer::Position::Type::PluginAuxIO,
                            .id = *mixer.instrumentAuxInputID(
                                indices[NodeData::Indent::ChannelIndex],
                                indices[NodeData::Indent::ChannelGroupIndex]
                            )
                        }
                    };
                }
                else
                {
                    return {
                        YADAW::Audio::Mixer::Mixer::Position {
                            .type = YADAW::Audio::Mixer::Mixer::Position::Type::PluginAuxIO,
                            .id = *mixer.instrumentAuxOutputID(
                                indices[NodeData::Indent::ChannelIndex],
                                indices[NodeData::Indent::ChannelGroupIndex]
                            )
                        }
                    };
                }
            }
            else if(position == NodeData::NodeInChannelPosition::PreFaderInserts)
            {
                auto mixerChannelListModel = mixerChannelListModels_[indices[NodeData::Indent::ListType]];
                auto& inserts = static_cast<MixerChannelInsertListModel*>(
                    mixerChannelListModel->data(
                        mixerChannelListModel->index(indices[NodeData::Indent::ChannelIndex]),
                        MixerChannelListModel::Role::Inserts
                    ).value<QObject*>()
                )->inserts();
                if(isInput_)
                {
                    return YADAW::Audio::Mixer::Mixer::Position {
                        .type = YADAW::Audio::Mixer::Mixer::Position::Type::PluginAuxIO,
                        .id = *inserts.insertAuxInputID(
                            indices[NodeData::Indent::InsertIndex],
                            indices[NodeData::Indent::ChannelGroupIndex]
                        )
                    };
                }
                else
                {
                    return YADAW::Audio::Mixer::Mixer::Position {
                        .type = YADAW::Audio::Mixer::Mixer::Position::Type::PluginAuxIO,
                        .id = *inserts.insertAuxOutputID(
                            indices[NodeData::Indent::InsertIndex],
                            indices[NodeData::Indent::ChannelGroupIndex]
                        )
                    };
                }
            }
        }
    }
    return std::nullopt;
}

QModelIndex MixerAudioIOPositionItemModel::findIndexByID(const QString& id) const
{
    return QModelIndex();
}

void MixerAudioIOPositionItemModel::initChildren(
    NodeData& nodeData, Indices& indices)
{
    auto& children = nodeData.children;
    if(nodeData.indent == NodeData::Indent::Root)
    {
        children.reserve(3);
        FOR_RANGE0(i, 3U)
        {
            children.emplace_back(
                std::make_unique<NodeData>(
                    NodeData {
                        .indent = NodeData::Indent::ListType,
                        .index = i,
                        .parent = &nodeData
                    }
                )
            );
        }
    }
    else if(nodeData.indent == NodeData::Indent::ListType)
    {
        auto listType = nodeData.index;
        auto count = static_cast<std::uint32_t>(
            mixerChannelListModels_[listType]->itemCount()
        );
        children.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& channelNode = children.emplace_back(
                std::make_unique<NodeData>(
                    NodeData {
                        .indent = NodeData::Indent::ChannelIndex,
                        .index  = i,
                        .parent = &nodeData
                    }
                )
            );
        }
    }
    else
    {
        auto& positions = nodeData.positions;
        auto listType = indices[NodeData::Indent::ListType];
        if(nodeData.indent == NodeData::Indent::ChannelIndex)
        {
            auto model = mixerChannelListModels_[listType];
            auto hasInstrument = model->data(
                model->index(nodeData.index),
                MixerChannelListModel::Role::InstrumentExist
            ).value<bool>();
            children.reserve(1 + hasInstrument);
            if(hasInstrument)
            {
                children.emplace_back(
                    std::make_unique<NodeData>(
                        NodeData {
                            .indent = NodeData::Indent::InChannelPosition,
                            .index  = NodeData::NodeInChannelPosition::Instrument,
                            .parent = &nodeData
                        }
                    )
                );
            }
            children.emplace_back(
                std::make_unique<NodeData>(
                    NodeData {
                        .indent = NodeData::Indent::InChannelPosition,
                        .index  = NodeData::NodeInChannelPosition::PreFaderInserts,
                        .parent = &nodeData
                    }
                )
            );
        }
        else if(nodeData.indent == NodeData::Indent::InChannelPosition)
        {
            auto model = mixerChannelListModels_[listType];
            if(nodeData.index == NodeData::NodeInChannelPosition::Instrument)
            {
                if(isInput_)
                {
                    auto instrumentAudioInputs = static_cast<AudioDeviceIOGroupListModel*>(
                        model->data(
                            model->index(indices[NodeData::Indent::ChannelIndex]),
                            MixerChannelListModel::Role::InstrumentAudioInputs
                        ).value<QObject*>()
                    );
                    auto count = static_cast<std::uint32_t>(instrumentAudioInputs->itemCount());
                    children.reserve(count);
                    positions.reserve(count);
                    FOR_RANGE0(j, count)
                    {
                        children.emplace_back(
                            std::make_unique<NodeData>(
                                NodeData {
                                    .indent = NodeData::Indent::ChannelGroupIndex,
                                    .index  = j,
                                    .parent = &nodeData
                                }
                            )
                        );
                        positions.emplace_back(
                            std::make_unique<YADAW::Entity::PluginAuxAudioIOPosition>(
                                createIndex(j, 0, &nodeData)
                            )
                        );
                    }
                }
                else
                {
                    auto instrumentAudioOutputs = static_cast<AudioDeviceIOGroupListModel*>(
                        model->data(
                            model->index(indices[NodeData::Indent::ChannelIndex]),
                            MixerChannelListModel::Role::InstrumentAudioOutputs
                        ).value<QObject*>()
                    );
                    auto count = static_cast<std::uint32_t>(instrumentAudioOutputs->itemCount());
                    children.reserve(count - 1);
                    positions.reserve(count - 1);
                    auto insertModel = static_cast<MixerChannelInsertListModel*>(
                        mixerChannelListModels_[listType]->data(
                            mixerChannelListModels_[listType]->index(
                                indices[NodeData::Indent::ChannelIndex]
                            ),
                            MixerChannelListModel::Role::Inserts
                        ).value<QObject*>()
                    );
                    auto mainOutput = insertModel->inserts().inChannelGroupIndex();
                    FOR_RANGE0(j, mainOutput)
                    {
                        children.emplace_back(
                            std::make_unique<NodeData>(
                                NodeData {
                                    .indent = NodeData::Indent::ChannelGroupIndex,
                                    .index  = j,
                                    .parent = &nodeData
                                }
                            )
                        );
                    }
                    FOR_RANGE(j, mainOutput + 1, count)
                    {
                        children.emplace_back(
                            std::make_unique<NodeData>(
                                NodeData {
                                    .indent = NodeData::Indent::ChannelGroupIndex,
                                    .index  = j,
                                    .parent = &nodeData
                                }
                            )
                        );
                    }
                    FOR_RANGE0(j, count - 1)
                    {
                        positions.emplace_back(
                            std::make_unique<YADAW::Entity::PluginAuxAudioIOPosition>(
                                createIndex(j, 0, &nodeData)
                            )
                        );
                    }
                }
            }
            else if(nodeData.index == NodeData::NodeInChannelPosition::PreFaderInserts)
            {
                auto insertModel = static_cast<MixerChannelInsertListModel*>(
                    model->data(
                        model->index(indices[NodeData::Indent::ChannelIndex]),
                        MixerChannelListModel::Role::Inserts
                    ).value<QObject*>()
                );
                auto count = static_cast<std::uint32_t>(insertModel->itemCount());
                children.reserve(count);
                FOR_RANGE0(j, count)
                {
                    children.emplace_back(
                        std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::InsertIndex,
                                .index  = j,
                                .parent = &nodeData
                            }
                        )
                    );
                }
            }
        }
        else if(nodeData.indent == NodeData::Indent::InsertIndex)
        {
            auto model = mixerChannelListModels_[listType];
            auto insertModel = static_cast<MixerChannelInsertListModel*>(
                model->data(
                    model->index(indices[NodeData::Indent::ChannelIndex]),
                    MixerChannelListModel::Role::Inserts
                ).value<QObject*>()
            );
            auto list = static_cast<AudioDeviceIOGroupListModel*>(
                insertModel->data(
                    insertModel->index(indices[NodeData::Indent::InsertIndex]),
                    isInput_?
                        MixerChannelInsertListModel::Role::AudioInputs:
                        MixerChannelInsertListModel::Role::AudioOutputs
                    )
                .value<QObject*>()
            );
            const auto& inserts = insertModel->inserts();
            auto mainChannelGroupIndex = isInput_?
                *inserts.insertInputChannelGroupIndexAt(indices[NodeData::Indent::InsertIndex]):
                *inserts.insertOutputChannelGroupIndexAt(indices[NodeData::Indent::InsertIndex]);
            std::generate_n(
                std::back_inserter(nodeData.children),
                mainChannelGroupIndex,
                [&nodeData, channelGroupIndex = 0U]() mutable
                {
                    return std::make_unique<NodeData>(
                        NodeData {
                            .indent = NodeData::Indent::ChannelGroupIndex,
                            .index  = channelGroupIndex++,
                            .parent = &nodeData
                        }
                    );
                }
            );
            std::generate_n(
                std::back_inserter(nodeData.children),
                list->itemCount() - mainChannelGroupIndex - 1,
                [&nodeData, channelGroupIndex = mainChannelGroupIndex + 1]() mutable
                {
                    return std::make_unique<NodeData>(
                        NodeData {
                            .indent = NodeData::Indent::ChannelGroupIndex,
                            .index  = channelGroupIndex++,
                            .parent = &nodeData
                        }
                    );
                }
            );
            std::generate_n(
                std::back_inserter(positions),
                list->itemCount() - 1,
                [this, &nodeData, channelGroupIndex = 0U]() mutable
                {
                    return std::make_unique<YADAW::Entity::PluginAuxAudioIOPosition>(
                        createIndex(channelGroupIndex++, 0, &nodeData)
                    );
                }
            );
        }
    }
    if(nodeData.indent != NodeData::Indent::Root)
    {
        indices[nodeData.indent] = nodeData.index;
    }
    for(auto& child: children)
    {
        initChildren(*child, indices);
    }
}

void MixerAudioIOPositionItemModel::updatePositionCompleteNames(NodeData& nodeData)
{
    for(auto& position: nodeData.positions)
    {
        position->completeNameChanged();
    }
    for(auto& child: nodeData.children)
    {
        updatePositionCompleteNames(*child);
    }
}

const MixerAudioIOPositionItemModel::NodeData*
MixerAudioIOPositionItemModel::getParentNodeData(const QModelIndex& index) const
{
    if(index == QModelIndex())
    {
        return nullptr;
    }
    return static_cast<const MixerAudioIOPositionItemModel::NodeData*>(index.constInternalPointer());
}

MixerAudioIOPositionItemModel::NodeData*
MixerAudioIOPositionItemModel::getParentNodeData(const QModelIndex& index)
{
    if(index == QModelIndex())
    {
        return nullptr;
    }
    return static_cast<MixerAudioIOPositionItemModel::NodeData*>(index.internalPointer());
}

const MixerAudioIOPositionItemModel::NodeData*
MixerAudioIOPositionItemModel::getNodeData(const QModelIndex& index) const
{
    auto parentNode = getParentNodeData(index);
    if(!parentNode)
    {
        return &rootNode_;
    }
    return parentNode->children[index.row()].get();
}

MixerAudioIOPositionItemModel::NodeData* MixerAudioIOPositionItemModel::getNodeData(const QModelIndex& index)
{
    auto parentNode = getParentNodeData(index);
    if(!parentNode)
    {
        return &rootNode_;
    }
    return parentNode->children[index.row()].get();
}
}
