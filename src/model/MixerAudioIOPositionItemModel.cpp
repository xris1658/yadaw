#include "MixerAudioIOPositionItemModel.hpp"

#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/MixerChannelInsertListModel.hpp"

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
    FOR_RANGE0(i, 3)
    {
        auto& vec = connectInsertToThis_[i];
        auto count = mixerChannelListModels_[i]->itemCount();
        vec.reserve(count);
        auto parentIndex = MixerAudioIOPositionItemModel::index(i, 0, QModelIndex());
        std::generate_n(
            std::back_inserter(vec), count,
            [this, parentIndex, channelList = mixerChannelListModels_[i], index = 0]() mutable
            {
                auto ret = std::make_unique<Impl::ConnectInsertToAudioIOPosition>(
                    *static_cast<MixerChannelInsertListModel*>(
                        channelList->data(
                            channelList->index(index),
                            MixerChannelListModel::Role::Inserts
                        ).value<QObject*>()
                    ),
                    *this,
                    MixerAudioIOPositionItemModel::index(
                        channelList->data(
                            channelList->index(index),
                            MixerChannelListModel::Role::InstrumentExist
                        ).value<bool>()? 1: 0, 0,
                        MixerAudioIOPositionItemModel::index(index, 0, parentIndex)
                    )
                );
                ++index;
                return ret;
            }
        );
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::rowsAboutToBeInserted,
            [this, parentIndex](const QModelIndex&, int first, int last)
            {
                beginInsertRows(parentIndex, first, last);
            }
        );
        QObject::connect(
            mixerChannelListModels_[i],
            &MixerChannelListModel::rowsInserted,
            [this, parentIndex, sender = mixerChannelListModels_[i], &treeNode = rootNode_.children[i], &vec]
            (const QModelIndex&, int first, int last) // rowsInserted
            {
                std::generate_n(
                    std::inserter(treeNode->children, treeNode->children.begin() + first), last - first + 1,
                    [&treeNode, channelIndex = static_cast<std::uint32_t>(first)]() mutable
                    {
                        return std::make_unique<NodeData>(
                            NodeData {
                                .indent = NodeData::Indent::ChannelIndex,
                                .index = channelIndex++,
                                .parent = treeNode.get()
                            }
                        );
                    }
                );
                std::generate_n(
                    std::inserter(vec, vec.begin() + first), last - first + 1,
                    [this, parentIndex, sender, channelIndex = first]() mutable
                    {
                        auto ret = std::make_unique<Impl::ConnectInsertToAudioIOPosition>(
                            *static_cast<MixerChannelInsertListModel*>(
                                sender->data(
                                    sender->index(channelIndex),
                                    MixerChannelListModel::Role::Inserts
                                ).value<QObject*>()
                            ),
                            *this,
                            MixerAudioIOPositionItemModel::index(
                                sender->data(
                                    sender->index(channelIndex),
                                    MixerChannelListModel::Role::InstrumentExist
                                ).value<bool>()? 1: 0, 0,
                                MixerAudioIOPositionItemModel::index(channelIndex, 0, parentIndex)
                            )
                        );
                        ++channelIndex;
                        return ret;
                    }
                );
                FOR_RANGE(j, last + 1, vec.size())
                {
                    vec[j]->updateIndex(
                        MixerAudioIOPositionItemModel::index(
                            sender->data(
                                sender->index(j),
                                MixerChannelListModel::Role::InstrumentExist
                            ).value<bool>()? 1: 0, 0,
                            MixerAudioIOPositionItemModel::index(j, 0, parentIndex)
                        )
                    );
                }
                endInsertRows();
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
            [this, parentIndex, channelList = mixerChannelListModels_[i], &treeNode = rootNode_.children[i], &vec]
            (const QModelIndex& parent, int first, int last) // rowsRemoved
            {
                treeNode->children.erase(
                    treeNode->children.begin() + first,
                    treeNode->children.begin() + last + 1
                );
                vec.erase(
                    vec.begin() + first,
                    vec.begin() + last + 1
                );
                FOR_RANGE(j, first, vec.size())
                {
                    vec[j]->updateIndex(
                        MixerAudioIOPositionItemModel::index(
                            channelList->data(
                                channelList->index(j),
                                MixerChannelListModel::Role::InstrumentExist
                            ).value<bool>()? 1: 0, 0,
                            MixerAudioIOPositionItemModel::index(j, 0, parentIndex)
                        )
                    );
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
                    auto& connectInsertToThis = connectInsertToThis_[1];
                    FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
                    {
                        auto instrumentExists = sender->data(
                            sender->index(i), MixerChannelListModel::Role::InstrumentExist
                        ).value<bool>();
                        auto oldRowCount = MixerAudioIOPositionItemModel::rowCount(
                            MixerAudioIOPositionItemModel::index(i, 0, parentIndex)
                        );
                        if(instrumentExists && oldRowCount == 2)
                        {
                            beginInsertRows(parentIndex, 0, 0);
                            connectInsertToThis[i]->updateIndex(
                                MixerAudioIOPositionItemModel::index(
                                    1, 0,
                                    MixerAudioIOPositionItemModel::index(
                                        i, 0, parentIndex
                                    )
                                )
                            );
                            endInsertRows();
                        }
                        else if(!instrumentExists && oldRowCount == 3)
                        {
                            beginRemoveRows(parentIndex, 0, 0);
                            connectInsertToThis[i]->updateIndex(
                                MixerAudioIOPositionItemModel::index(
                                    0, 0,
                                    MixerAudioIOPositionItemModel::index(
                                        i, 0, parentIndex
                                    )
                                )
                            );
                            endInsertRows();
                        }
                    }
                }
            }
        }
    );
}

QModelIndex MixerAudioIOPositionItemModel::index(
    int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, getNodeData(parent));
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
    auto parentNode = getNodeData(parent);
    if(parentNode->indent == NodeData::Indent::InChannelPosition)
    {
        return parentNode->index != NodeData::NodeInChannelPosition::Instrument;
    }
    return parentNode->indent != NodeData::Indent::InsertIndex;
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

std::unique_ptr<MixerAudioIOPositionItemModel::NodeData>
MixerAudioIOPositionItemModel::createNode(
    MixerChannelListModel::ListType type)
{
    auto ret = std::make_unique<NodeData>(
        NodeData {
            .indent = NodeData::Indent::ListType,
            .index  = static_cast<std::uint32_t>(type),
            .parent = &rootNode_
        }
    );
    ret->children.reserve(mixerChannelListModels_[type]->itemCount());
    FOR_RANGE0(i, static_cast<std::uint32_t>(mixerChannelListModels_[type]->itemCount()))
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
                auto count = static_cast<std::uint32_t>(instrumentAudioInputs->itemCount());
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
                    ).value<QObject*>()
                );
                auto count = instrumentAudioOutputs->itemCount();
                instrumentNode->children.reserve(count - 1);
                auto insertModel = static_cast<MixerChannelInsertListModel*>(
                    mixerChannelListModels_[type]->data(
                        mixerChannelListModels_[type]->index(i),
                        MixerChannelListModel::Role::Inserts
                    ).value<QObject*>()
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
        auto insertCount = static_cast<std::uint32_t>(insertListModel->itemCount());
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
