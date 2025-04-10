#if _WIN32

#include "AudioGraphInputDeviceListModel.hpp"

#include "audio/backend/AudioGraphBackend.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AudioGraphInputDeviceListModel::AudioGraphInputDeviceListModel(Audio::Backend::AudioGraphBackend& backend,
    QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{
    YADAW::Util::setCppOwnership(*this);
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
        case Role::Enabled:
            return QVariant::fromValue(backend_->isDeviceInputActivated(row));
        case Role::ChannelCount:
            return QVariant::fromValue<int>(backend_->channelCount(true, row));
        }
    }
    return {};
}

bool AudioGraphInputDeviceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Enabled:
        {
            auto ret = backend_->activateDeviceInput(row, value.value<bool>()) == S_OK;
            if(ret)
            {
                dataChanged(index, index, {Role::Enabled});
            }
            return ret;
        }
        }
    }
    return false;
}
}

#endif