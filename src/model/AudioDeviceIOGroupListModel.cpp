#include "AudioDeviceIOGroupListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
AudioDeviceIOGroupListModel::AudioDeviceIOGroupListModel(
    const YADAW::Audio::Device::IAudioDevice& audioDevice, bool isInput,
    QObject* parent):
    IAudioDeviceIOGroupListModel(parent),
    device_(&audioDevice),
    speakerListModel_(),
    isInput_(isInput)
{
    const auto count = itemCount();
    speakerListModel_.reserve(count);
    FOR_RANGE0(i, count)
    {
        auto& channelGroup = (
            isInput_?
                device_->audioInputGroupAt(i):
                device_->audioOutputGroupAt(i)
            )->get();
        auto& speakerListModel = speakerListModel_.emplace_back(channelGroup);
        speakerListModel.setParent(this);
    }
}

AudioDeviceIOGroupListModel::~AudioDeviceIOGroupListModel()
{}

int AudioDeviceIOGroupListModel::itemCount() const
{
    return isInput_? device_->audioInputGroupCount(): device_->audioOutputGroupCount();
}

int AudioDeviceIOGroupListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AudioDeviceIOGroupListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto group = isInput_? device_->audioInputGroupAt(row): device_->audioOutputGroupAt(row);
        if(group.has_value())
        {
            switch(role)
            {
            case Role::Name:
                return QVariant::fromValue(group->get().name());
            case Role::IsMain:
                return QVariant::fromValue(group->get().isMain());
            case Role::ChannelGroupType:
                return QVariant::fromValue(static_cast<int>(group->get().type()) + 1);
            case Role::SpeakerList:
                return QVariant::fromValue<QObject*>(&speakerListModel_[row]);
            case Role::ChannelCount:
                return QVariant::fromValue(group->get().channelCount());
            }
        }
    }
    return {};
}
}