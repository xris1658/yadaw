#include "AudioBusChannelListModel.hpp"

#include "model/AudioBusConfigurationModel.hpp"

namespace YADAW::Model
{
using YADAW::Audio::Device::IAudioBusConfiguration;

AudioBusChannelListModel::AudioBusChannelListModel(
    IAudioBusConfiguration& configuration,
    AudioBusConfigurationModel& configurationModel,
    bool isInput,
    std::uint32_t index, QObject* parent):
    IAudioBusChannelListModel(parent),
    configuration_(&configuration),
    configurationModel_(&configurationModel),
    bus_(
        isInput?
        (&configuration.inputBusAt(index)->get()):
        (&configuration.outputBusAt(index)->get())
    )
{}

AudioBusChannelListModel::AudioBusChannelListModel(const AudioBusChannelListModel& rhs):
    IAudioBusChannelListModel(rhs.parent()),
    configuration_(rhs.configuration_),
    configurationModel_(rhs.configurationModel_),
    bus_(rhs.bus_)
{}

AudioBusChannelListModel& AudioBusChannelListModel::operator=(const AudioBusChannelListModel& rhs)
{
    if(this != &rhs)
    {
        configuration_ = rhs.configuration_;
        configurationModel_ = rhs.configurationModel_;
        bus_ = rhs.bus_;
    }
    return *this;
}

AudioBusChannelListModel::~AudioBusChannelListModel()
{}

std::uint32_t AudioBusChannelListModel::itemCount() const
{
    return bus_->channelCount();
}

int AudioBusChannelListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AudioBusChannelListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto channel = bus_->channelAt(row).value();
        switch(role)
        {
        case Role::ChannelIndex:
        {
            auto value = channel.channelIndex;
            if(value == YADAW::Audio::Device::InvalidIndex)
            {
                return QVariant::fromValue<int>(-1);
            }
            return QVariant::fromValue<int>(value);
        }
        case Role::DeviceIndex:
        {
            auto value = channel.deviceIndex;
            if(value == YADAW::Audio::Device::InvalidIndex)
            {
                return QVariant::fromValue<int>(-1);
            }
            return QVariant::fromValue<int>(value);
        }
        }
    }
    return {};
}

bool AudioBusChannelListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& channel = bus_->channelAt(row).value();
        switch(role)
        {
        case Role::ChannelIndex:
        {
            return setChannel(row, channel.deviceIndex, value.value<int>());
        }
        case Role::DeviceIndex:
        {
            return setChannel(row, value.value<int>(), channel.channelIndex);
        }
        }
    }
    return false;
}

bool AudioBusChannelListModel::setChannel(std::uint32_t index,
    std::uint32_t deviceIndex, std::uint32_t channelIndex)
{
    if(index < itemCount())
    {
        auto ret = bus_->setChannel(index, {deviceIndex, channelIndex});
        if(ret)
        {
            dataChanged(
                this->index(index), this->index(index),
                {Role::DeviceIndex, Role::ChannelIndex}
            );
            return true;
        }
    }
    return false;
}
}