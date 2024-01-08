#ifndef YADAW_SRC_MODEL_DEVICEIOGROUPLISTMODEL
#define YADAW_SRC_MODEL_DEVICEIOGROUPLISTMODEL

#include "model/IDeviceIOGroupListModel.hpp"

#include "audio/device/IAudioDevice.hpp"

namespace YADAW::Model
{
class DeviceIOGroupListModel: public IDeviceIOGroupListModel
{
    Q_OBJECT
public:
    DeviceIOGroupListModel(const YADAW::Audio::Device::IAudioDevice* audioDevice,
        bool isInput,
        QObject* parent = nullptr);
    ~DeviceIOGroupListModel();
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const YADAW::Audio::Device::IAudioDevice* device_;
    bool isInput_;
};
}

#endif //YADAW_SRC_MODEL_DEVICEIOGROUPLISTMODEL
