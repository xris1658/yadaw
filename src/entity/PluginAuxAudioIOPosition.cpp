#include "PluginAuxAudioIOPosition.hpp"

#include "model/MixerAudioIOPositionItemModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
PluginAuxAudioIOPosition::PluginAuxAudioIOPosition(
    const QModelIndex& index, QObject* parent):
    IAudioIOPosition(parent),
    index_(index)
{
    YADAW::Util::setCppOwnership(*this);
}

PluginAuxAudioIOPosition::~PluginAuxAudioIOPosition()
{}

IAudioIOPosition::Type PluginAuxAudioIOPosition::getType() const
{
    return IAudioIOPosition::Type::PluginAuxIO;
}

QString PluginAuxAudioIOPosition::getName() const
{
    return getModel().data(
        index_,
        YADAW::Model::MixerAudioIOPositionItemModel::Role::TreeName
    ).value<QString>();
}

constexpr char listTypeIndicator[3][2] = {"I", "R", "O"};

QString PluginAuxAudioIOPosition::getCompleteName() const
{
    using YADAW::Model::MixerAudioIOPositionItemModel;
    auto model = static_cast<const MixerAudioIOPositionItemModel*>(index_.model());
    auto node = model->getNodeData(index_);
    MixerAudioIOPositionItemModel::Indices indices;
    for(auto n = node; n != &model->rootNode_; n = n->parent)
    {
        indices[n->indent] = n->index;
    }
    QString ret;
    ret.append(listTypeIndicator[indices[MixerAudioIOPositionItemModel::NodeData::Indent::ListType]]);
    ret.append(QString::number(indices[MixerAudioIOPositionItemModel::NodeData::Indent::ChannelIndex] + 1));
    auto inChannelPosition = indices[MixerAudioIOPositionItemModel::NodeData::Indent::InChannelPosition];
    if(inChannelPosition == MixerAudioIOPositionItemModel::NodeData::NodeInChannelPosition::Instrument)
    {
        ret.append("Ins");
    }
    else
    {
        if(inChannelPosition == MixerAudioIOPositionItemModel::NodeData::NodeInChannelPosition::PreFaderInserts)
        {
            ret.append("Pre");
        }
        else if(inChannelPosition == MixerAudioIOPositionItemModel::NodeData::NodeInChannelPosition::PostFaderInserts)
        {
            ret.append("Pos");
        }
        ret.append(QString::number(indices[MixerAudioIOPositionItemModel::NodeData::Indent::InsertIndex] + 1));
        ret.append("-");
    }
    ret.append(QString::number(indices[MixerAudioIOPositionItemModel::NodeData::Indent::ChannelGroupIndex] + 1));
    return ret;
}

const YADAW::Model::MixerAudioIOPositionItemModel& PluginAuxAudioIOPosition::getModel() const
{
    return *static_cast<const YADAW::Model::MixerAudioIOPositionItemModel*>(index_.model());
}

PluginAuxAudioIOPosition::operator YADAW::Audio::Mixer::Mixer::Position() const
{
    return getModel().getPosition(index_).value_or(
        YADAW::Audio::Mixer::Mixer::Position {
            .type = YADAW::Audio::Mixer::Mixer::Position::Type::Invalid,
            .id = YADAW::Audio::Mixer::IDGen::InvalidId
        }
    );
}

void PluginAuxAudioIOPosition::updateIndex(const QModelIndex& index)
{
    index_ = index;
    nameChanged();
}

void PluginAuxAudioIOPosition::updateIndex(QModelIndex&& index)
{
    index_ = std::move(index);
    nameChanged();
}
}
