#include "SpeakerListModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
SpeakerListModel::SpeakerListModel()
{
    YADAW::Util::setCppOwnership(*this);
}

SpeakerListModel::SpeakerListModel(const Audio::Device::IAudioChannelGroup& group):
    ISpeakerListModel(),
    group_(&group)
{
    YADAW::Util::setCppOwnership(*this);
}

SpeakerListModel::SpeakerListModel(const SpeakerListModel& rhs):
    ISpeakerListModel(rhs.parent()),
    group_(rhs.group_)
{
    YADAW::Util::setCppOwnership(*this);
}

SpeakerListModel& SpeakerListModel::operator=(const SpeakerListModel& rhs)
{
    if(this != &rhs)
    {
        if(auto oldCount = itemCount(); oldCount != 0)
        {
            beginRemoveRows(QModelIndex(), 0, oldCount - 1);
            group_ = nullptr;
            endRemoveRows();
        }
    }
    group_ = rhs.group_;
    if(auto newCount = itemCount(); newCount != 0)
    {
        beginInsertRows(QModelIndex(), 0, newCount - 1);
        endInsertRows();
    }
    return *this;
}

SpeakerListModel::~SpeakerListModel()
{}

int SpeakerListModel::itemCount() const
{
    return group_? group_->channelCount(): 0;
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
