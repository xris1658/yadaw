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
    isInput_(isInput),
    index_(index)
{}

AudioBusChannelListModel::AudioBusChannelListModel(const AudioBusChannelListModel& rhs):
    IAudioBusChannelListModel(rhs.parent()),
    configuration_(rhs.configuration_),
    configurationModel_(rhs.configurationModel_),
    isInput_(rhs.isInput_),
    index_(rhs.index_)
{}

AudioBusChannelListModel::~AudioBusChannelListModel()
{}

std::uint32_t AudioBusChannelListModel::itemCount() const
{
    const auto& bus = isInput_? configuration_->inputBusAt(index_)->get():
        configuration_->outputBusAt(index_)->get();
    return bus.channelCount();
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
        auto channel = (isInput_?
            configuration_->inputBusAt(index_):
            configuration_->outputBusAt(index_)
        )->get().channelAt(row).value();
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
        const auto& channel =
            (isInput_?
                configuration_->inputBusAt(index_):
                configuration_->outputBusAt(index_))
            ->get().channelAt(row).value();
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
        auto& bus = (
            isInput_?
                configuration_->inputBusAt(index_):
                configuration_->outputBusAt(index_))
        ->get();
        auto ret = bus.setChannel(index, {deviceIndex, channelIndex});
        if(ret)
        {
            dataChanged(this->index(index), this->index(index),
                {Role::DeviceIndex, Role::ChannelIndex});
            configurationModel_->dataChanged(
                configurationModel_->index(index_, 0),
                configurationModel_->index(index_, 0),
                {IAudioBusConfigurationModel::Role::ChannelList});
            return true;
        }
    }
    return false;
}
}