#include "DeviceIOGroupListModel.hpp"

namespace YADAW::Model
{
DeviceIOGroupListModel::DeviceIOGroupListModel(
    const YADAW::Audio::Device::IAudioDevice* audioDevice, bool isInput,
    QObject* parent):
    IDeviceIOGroupListModel(parent),
    device_(audioDevice),
    isInput_(isInput)
{}

DeviceIOGroupListModel::~DeviceIOGroupListModel()
{}

int DeviceIOGroupListModel::itemCount() const
{
    return isInput_? device_->audioInputGroupCount(): device_->audioOutputGroupCount();
}

int DeviceIOGroupListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant DeviceIOGroupListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto group = isInput_? device_->audioInputGroupAt(row): device_->audioOutputGroupAt(row);
        if(group.has_value())
        {
            switch(role)
            {
            case Role::Name:
                return group->get().name();
            case Role::IsMain:
                return group->get().isMain();
            }
        }
    }
    return {};
}
}