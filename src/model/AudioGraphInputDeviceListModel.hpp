#ifndef YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL
#define YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/IAudioGraphDeviceListModel.hpp"

namespace YADAW::Model
{
class AudioGraphInputDeviceListModel: public IAudioGraphDeviceListModel
{
    Q_OBJECT
public:
    AudioGraphInputDeviceListModel(const YADAW::Audio::Backend::AudioGraphBackend& backend,
        QObject* parent = nullptr);
    ~AudioGraphInputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const YADAW::Audio::Backend::AudioGraphBackend* backend_;
};
}

#endif //YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL
