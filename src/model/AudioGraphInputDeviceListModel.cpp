#include "AudioGraphInputDeviceListModel.hpp"

namespace YADAW::Model
{
AudioGraphInputDeviceListModel::AudioGraphInputDeviceListModel(const Audio::Backend::AudioGraphBackend& backend,
    QObject* parent):
    IAudioGraphDeviceListModel(parent),
    backend_(&backend)
{
    //
}

AudioGraphInputDeviceListModel::~AudioGraphInputDeviceListModel()
{
}

int AudioGraphInputDeviceListModel::itemCount() const
{
    return backend_->audioInputDeviceCount();
}

int AudioGraphInputDeviceListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AudioGraphInputDeviceListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& deviceInfo = backend_->audioInputDeviceAt(row);
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(deviceInfo.id);
        case Role::Name:
            return QVariant::fromValue(deviceInfo.name);
        }
    }
    return {};
}
}