#ifndef YADAW_ALSAINPUTDEVICELISTMODEL_HPP
#define YADAW_ALSAINPUTDEVICELISTMODEL_HPP

#if(__linux__)

#include "audio/backend/ALSABackend.hpp"
#include "model/IAudioDeviceListModel.hpp"

namespace YADAW::Model
{
class ALSAInputDeviceListModel: public IAudioDeviceListModel
{
    Q_OBJECT
public:
    ALSAInputDeviceListModel(YADAW::Audio::Backend::ALSABackend& backend,
        QObject* parent = nullptr);
    ~ALSAInputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    YADAW::Audio::Backend::ALSABackend* backend_;
};
}

#endif

#endif //YADAW_ALSAINPUTDEVICELISTMODEL_HPP
