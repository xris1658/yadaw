#include "SpeakerListModel.hpp"

namespace YADAW::Model
{
SpeakerListModel::SpeakerListModel(const Audio::Device::IAudioChannelGroup& group):
    ISpeakerListModel(),
    group_(&group)
{}

SpeakerListModel::SpeakerListModel(const SpeakerListModel& rhs):
    group_(rhs.group_)
{}

SpeakerListModel::SpeakerListModel(SpeakerListModel&& rhs):
    group_(rhs.group_)
{}

SpeakerListModel::~SpeakerListModel()
{}

int SpeakerListModel::itemCount() const
{
    return group_->channelCount();
}

int SpeakerListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant SpeakerListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            return QVariant::fromValue(group_->speakerNameAt(row));
        case Role::Type:
            return QVariant::fromValue(group_->speakerAt(row));
        }
    }
    return {};
}
}