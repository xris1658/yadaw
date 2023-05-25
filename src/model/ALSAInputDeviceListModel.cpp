#if(__linux__)

#include "ALSAInputDeviceListModel.hpp"

namespace YADAW::Model
{
ALSAInputDeviceListModel::ALSAInputDeviceListModel(
    Audio::Backend::ALSAAudioBackend& backend, QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{}

ALSAInputDeviceListModel::~ALSAInputDeviceListModel()
{}

int ALSAInputDeviceListModel::itemCount() const
{
    return backend_->audioInputDeviceCount();
}

int ALSAInputDeviceListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant ALSAInputDeviceListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto selector = backend_->audioInputDeviceAt(row).value();
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(QString("hw:%1,%2").arg(selector.cIndex, selector.dIndex));
        case Role::Name:
            return QVariant::fromValue(QString());
        case Role::Enabled:
            return backend_->isAudioInputDeviceActivated(selector);
        }
    }
    return {};
}

bool ALSAInputDeviceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}
}

#endif