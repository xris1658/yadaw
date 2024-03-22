#if __linux__

#include "ALSAInputDeviceListModel.hpp"

namespace YADAW::Model
{
ALSAInputDeviceListModel::ALSAInputDeviceListModel(
    YADAW::Audio::Backend::ALSABackend& backend, QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{}

ALSAInputDeviceListModel::~ALSAInputDeviceListModel()
{}

std::uint32_t ALSAInputDeviceListModel::itemCount() const
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
        {
            auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(selector.cIndex).value_or(std::string("Card ") + std::to_string(selector.cIndex));
            auto deviceName = YADAW::Audio::Backend::ALSABackend::audioDeviceName(selector).value_or(std::string("Device ") + std::to_string(selector.dIndex));
            auto name = deviceName + std::string(" (") + cardName + std::string(")");
            return QVariant::fromValue(QString::fromStdString(name));
        }
        case Role::Enabled:
            return backend_->isAudioDeviceActivated(true, row);
        case Role::ChannelCount:
            return QVariant::fromValue<int>(backend_->channelCount(true, row));
        }
    }
    return {};
}

bool ALSAInputDeviceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Enabled:
        {
            if(auto result = backend_->setAudioDeviceActivated(true, row, value.value<bool>());
                result.second == 0)
            {
                dataChanged(index, index, {Role::Enabled});
                return true;
            }
        }
        }
    }
    return false;
}
}

#endif