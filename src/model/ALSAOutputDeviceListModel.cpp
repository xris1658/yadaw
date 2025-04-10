#if __linux__

#include "ALSAOutputDeviceListModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
ALSAOutputDeviceListModel::ALSAOutputDeviceListModel(
    Audio::Backend::ALSABackend& backend, QObject* parent):
    IAudioDeviceListModel(parent),
    backend_(&backend)
{
    YADAW::Util::setCppOwnership(*this);
}

ALSAOutputDeviceListModel::~ALSAOutputDeviceListModel()
{}

std::uint32_t ALSAOutputDeviceListModel::itemCount() const
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
        {
            auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(selector.cIndex).value_or(std::string("Card ") + std::to_string(selector.cIndex));
            auto deviceName = YADAW::Audio::Backend::ALSABackend::audioDeviceName(selector).value_or(std::string("Device ") + std::to_string(selector.dIndex));
            auto name = deviceName + std::string(" (") + cardName + std::string(")");
            return QVariant::fromValue(QString::fromStdString(name));
        }
        case Role::Enabled:
            return backend_->isAudioDeviceActivated(false, row);
        case Role::ChannelCount:
            return QVariant::fromValue<int>(backend_->channelCount(false, row));
        }
    }
    return {};
}

bool ALSAOutputDeviceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Enabled:
        {
            if(auto result = backend_->setAudioDeviceActivated(false, row, value.value<bool>());
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