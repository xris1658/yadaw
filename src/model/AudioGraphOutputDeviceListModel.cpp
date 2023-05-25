#if(WIN32)

#include "AudioGraphOutputDeviceListModel.hpp"

namespace YADAW::Model
{
AudioGraphOutputDeviceListModel::AudioGraphOutputDeviceListModel(
    YADAW::Audio::Backend::AudioGraphBackend& backend,
    QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{
}

AudioGraphOutputDeviceListModel::~AudioGraphOutputDeviceListModel()
{
}

int AudioGraphOutputDeviceListModel::itemCount() const
{
    return backend_->audioOutputDeviceCount();
}

int AudioGraphOutputDeviceListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AudioGraphOutputDeviceListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& deviceInfo = backend_->audioOutputDeviceAt(row);
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(deviceInfo.id);
        case Role::Name:
            return QVariant::fromValue(deviceInfo.name);
        case Role::Enabled:
        {
            const auto& currentOutputDeviceInfo = backend_->currentOutputDevice();
            return QVariant::fromValue(currentOutputDeviceInfo.id == deviceInfo.id);
        }
        case Role::ChannelCount:
            return QVariant::fromValue<int>(backend_->channelCount(false, row));
        }
    }
    return {};
}

bool AudioGraphOutputDeviceListModel::setOutputDeviceIndex(std::uint32_t index)
{
    auto ret = backend_->setOutputDeviceIndex(index);
    if(ret)
    {
        dataChanged(this->index(0), this->index(itemCount() - 1),
            {Role::Enabled});
        return true;
    }
    return false;
}
}

#endif