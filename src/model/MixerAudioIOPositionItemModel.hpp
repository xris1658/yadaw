#ifndef YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL
#define YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL

#include "model/IAudioIOPositionItemModel.hpp"

#include "entity/PluginAuxAudioIOPosition.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Model
{
class MixerAudioIOPositionItemModel: public IAudioIOPositionItemModel
{
    Q_OBJECT
    friend YADAW::Entity::PluginAuxAudioIOPosition;
private:
    struct NodeData
    {
        enum Indent: std::int8_t
        {
            Root = -1,
            ListType,
            ChannelIndex,
            InChannelPosition,
            InsertIndex,
            ChannelGroupIndex,
            Count
        };
        enum NodeInChannelPosition: std::uint8_t
        {
            Instrument,
            PreFaderInserts,
            PostFaderInserts
        };
        std::int8_t indent;
        std::uint32_t index;
        NodeData* parent = nullptr;
        std::vector<std::unique_ptr<NodeData>> children;
        std::vector<std::unique_ptr<YADAW::Entity::PluginAuxAudioIOPosition>> positions;
    };
public:
    MixerAudioIOPositionItemModel(
        MixerChannelListModel& mixerAudioInputChannelListModel,
        MixerChannelListModel& mixerRegularChannelListModel,
        MixerChannelListModel& mixerAudioOutputChannelListModel,
        bool isInput);
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    bool hasChildren(const QModelIndex& parent) const override;
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
public:
    std::optional<YADAW::Audio::Mixer::Mixer::Position> getPosition(const QModelIndex& index) const;
    QModelIndex findIndexByID(const QString& id) const override;
private:
    using Indices = std::uint8_t[NodeData::Indent::Count];
    void initChildren(NodeData& nodeData, Indices& indices);
    void updatePositionCompleteNames(NodeData& nodeData);
    const NodeData* getParentNodeData(const QModelIndex& index) const;
    NodeData* getParentNodeData(const QModelIndex& index);
    const NodeData* getNodeData(const QModelIndex& index) const;
    NodeData* getNodeData(const QModelIndex& index);
private:
    MixerChannelListModel* mixerChannelListModels_[3];
    bool isInput_;
    NodeData rootNode_ {
        .indent = NodeData::Indent::Root,
        .index = 0,
        .parent = nullptr
    };
};
}

#endif // YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL
