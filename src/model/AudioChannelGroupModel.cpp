#include "AudioChannelGroupModel.hpp"

#include "util/Base.hpp"

#include <iterator>

namespace YADAW::Model
{
using namespace YADAW::Audio::Base;
using YADAW::Audio::Device::IAudioChannelGroup;

IAudioChannelGroupModel::Channel channelType[] = {
    IAudioChannelGroupModel::Channel::Center,
    IAudioChannelGroupModel::Channel::Left,
    IAudioChannelGroupModel::Channel::Right,
    IAudioChannelGroupModel::Channel::SideLeft,
    IAudioChannelGroupModel::Channel::SideRight,
    IAudioChannelGroupModel::Channel::RearLeft,
    IAudioChannelGroupModel::Channel::RearRight,
    IAudioChannelGroupModel::Channel::RearCenter,
    IAudioChannelGroupModel::Channel::LFE
};

AudioChannelGroupModel::AudioChannelGroupModel(const IAudioChannelGroup& group,
    QObject* parent):
    IAudioChannelGroupModel(parent),
    group_(&group)
{}

AudioChannelGroupModel::~AudioChannelGroupModel()
{}

int AudioChannelGroupModel::itemCount() const
{
    return group_->channelCount();
}

int AudioChannelGroupModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant AudioChannelGroupModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::ChannelType:
        {
            auto type = YADAW::Util::underlyingValue(group_->speakerAt(row));
            if(type >= 0 && type < std::size(channelType))
            {
                return QVariant::fromValue<IAudioChannelGroupModel::Channel>(channelType[type]);
            }
            return {};
        }
        case Role::Name:
            return QVariant::fromValue<QString>(group_->speakerNameAt(row));
        }
    }
    return {};
}
}
