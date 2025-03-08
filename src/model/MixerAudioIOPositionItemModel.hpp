#ifndef YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL
#define YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL

#include "model/IAudioIOPositionItemModel.hpp"

#include "model/MixerChannelListModel.hpp"
#include "model/impl/ConnectInsertToAudioIOPosition.hpp"

namespace YADAW::Model
{
class MixerAudioIOPositionItemModel: public IAudioIOPositionItemModel
{
    Q_OBJECT
private:
    struct NodeData
    {
        enum Indent: std::uint8_t
        {
            ListType,
            ChannelIndex,
            InChannelPosition,
            InsertIndex,
            ChannelGroupIndex,
            Count = InsertIndex + 1
        };
        enum NodeInChannelPosition: std::uint8_t
        {
            Instrument,
            PreFaderInserts,
            PostFaderInserts
        };
        std::uint8_t indent;
        std::uint32_t index;
        NodeData* parent = nullptr;
        std::vector<std::unique_ptr<NodeData>> children;
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
    QModelIndex findIndexByID(const QString& id) const override;
private:
    std::unique_ptr<NodeData> createNode(
        MixerChannelListModel::ListType type
    );
    const NodeData* getNodeData(const QModelIndex& index) const;
    NodeData* getNodeData(const QModelIndex& index);
private:
    MixerChannelListModel* mixerChannelListModels_[3];
    std::vector<std::unique_ptr<Impl::ConnectInsertToAudioIOPosition>> connectInsertToThis_[3];
    bool isInput_;
    NodeData rootNode_;
};
}

#endif // YADAW_SRC_MODEL_MIXERAUDIOIOPOSITIONITEMMODEL
