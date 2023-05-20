#ifndef YADAW_ALSAOUTPUTDEVICELISTMODEL_HPP
#define YADAW_ALSAOUTPUTDEVICELISTMODEL_HPP

#include "audio/backend/ALSAAudioBackend.hpp"
#include "model/IAudioDeviceListModel.hpp"

namespace YADAW::Model
{
class ALSAOutputDeviceListModel: public IAudioDeviceListModel
{
    Q_OBJECT
public:
    ALSAOutputDeviceListModel(YADAW::Audio::Backend::ALSAAudioBackend& backend,
        QObject* parent = nullptr);
    ~ALSAOutputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    YADAW::Audio::Backend::ALSAAudioBackend* backend_;
};
}

#endif //YADAW_ALSAOUTPUTDEVICELISTMODEL_HPP
