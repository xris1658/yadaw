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
        speakerListModel_.emplace_back(channelGroup);
    }
}

AudioDeviceIOGroupListModel::AudioDeviceIOGroupListModel(
    const AudioDeviceIOGroupListModel& rhs):
    IAudioDeviceIOGroupListModel(rhs.parent()),
    device_(rhs.device_),
    speakerListModel_(rhs.speakerListModel_),
    isInput_(rhs.isInput_)
{}

AudioDeviceIOGroupListModel& AudioDeviceIOGroupListModel::operator=(
    const AudioDeviceIOGroupListModel& rhs)
{
    if(this != &rhs)
    {
        if(auto oldCount = itemCount(); oldCount != 0)
        {
            beginRemoveRows(QModelIndex(), 0, oldCount - 1);
            device_ = nullptr;
            endRemoveRows();
        }
        device_ = rhs.device_;
        speakerListModel_.clear();
        std::copy(rhs.speakerListModel_.begin(), rhs.speakerListModel_.end(),
            std::back_inserter(speakerListModel_));
        isInput_ = rhs.isInput_;
        if(auto newCount = itemCount(); newCount != 0)
        {
            beginInsertRows(QModelIndex(), 0, newCount - 1);
            endInsertRows();
        }
    }
    return *this;
}

AudioDeviceIOGroupListModel::~AudioDeviceIOGroupListModel()
{}

int AudioDeviceIOGroupListModel::itemCount() const
{
    return device_?
        isInput_?
            device_->audioInputGroupCount():
            device_->audioOutputGroupCount():
        0;
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