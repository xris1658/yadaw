#include "AudioBusConfigurationModel.hpp"

namespace YADAW::Model
{
AudioBusConfigurationModel::AudioBusConfigurationModel(
    YADAW::Audio::Device::IAudioBusConfiguration& configuration,
    bool isInput):
    configuration_(&configuration),
    channelList_(),
    name_(),
    isInput_(isInput)
{
    name_.resize(itemCount());
    for(std::uint32_t i = 0; i < itemCount(); ++i)
    {
        channelList_.emplace_back(configuration, isInput_, i);
    }
}

AudioBusConfigurationModel::~AudioBusConfigurationModel()
{}

int AudioBusConfigurationModel::itemCount() const
{
    return isInput_? configuration_->inputBusCount():
        configuration_->outputBusCount();
}

int AudioBusConfigurationModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant AudioBusConfigurationModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            return QVariant::fromValue(name_[row]);
        case Role::ChannelList:
            // Some crappy code right here
            return QVariant::fromValue<QObject*>(
                const_cast<YADAW::Model::AudioBusChannelListModel*>(
                    &channelList_[row]
                )
            );
        }
    }
    return {};
}

bool AudioBusConfigurationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            name_[row] = value.value<QString>();
            dataChanged(this->index(row), this->index(row), {Role::Name});
            return true;
        }
    }
    return false;
}

bool AudioBusConfigurationModel::append(int channelCount)
{
    if(channelCount <= 0)
    {
        return false;
    }
    beginInsertRows(QModelIndex(), itemCount(), itemCount());
    auto newIndex = configuration_->appendBus(isInput_, channelCount);
    channelList_.emplace_back(*configuration_, isInput_, newIndex);
    name_.emplace_back();
    endInsertRows();
    return true;
}

bool AudioBusConfigurationModel::remove(int index)
{
    if(index >= 0 && index < itemCount())
    {
        beginRemoveRows(QModelIndex(), index, index);
        name_.erase(name_.begin() + index);
        configuration_->removeBus(isInput_, index);
        channelList_.clear();
        for(std::uint32_t i = 0; i < itemCount(); ++i)
        {
            channelList_.emplace_back(*configuration_, isInput_, i);
        }
        endRemoveRows();
        return true;
    }
    return false;
}

void AudioBusConfigurationModel::clear()
{
    configuration_->clearBus(isInput_);
}
}