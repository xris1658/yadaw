#if(__linux__)

#include "ALSAOutputDeviceListModel.hpp"

namespace YADAW::Model
{
ALSAOutputDeviceListModel::ALSAOutputDeviceListModel(
    Audio::Backend::ALSAAudioBackend& backend, QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{}

ALSAOutputDeviceListModel::~ALSAOutputDeviceListModel()
{}

int ALSAOutputDeviceListModel::itemCount() const
{
    return backend_->audioOutputDeviceCount();
}

int ALSAOutputDeviceListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant ALSAOutputDeviceListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto selector = backend_->audioOutputDeviceAt(row).value();
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(QString("hw:%1,%2").arg(selector.cIndex, selector.dIndex));
        case Role::Name:
            return QVariant::fromValue(QString());
        case Role::Enabled:
            return backend_->isAudioOutputDeviceActivated(selector);
        }
    }
    return {};
}

bool ALSAOutputDeviceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}
}

#endif