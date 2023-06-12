#ifndef YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL
#define YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL

#if(WIN32)

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/IAudioDeviceListModel.hpp"

namespace YADAW::Model
{
class AudioGraphInputDeviceListModel: public IAudioDeviceListModel
{
    Q_OBJECT
public:
    AudioGraphInputDeviceListModel(YADAW::Audio::Backend::AudioGraphBackend& backend,
        QObject* parent = nullptr);
    ~AudioGraphInputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    YADAW::Audio::Backend::AudioGraphBackend* backend_;
};
}

#endif

#endif // YADAW_SRC_MODEL_AUDIOGRAPHINPUTDEVICELISTMODEL
