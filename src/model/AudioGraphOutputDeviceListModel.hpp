#ifndef YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL
#define YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/IAudioGraphDeviceListModel.hpp"

namespace YADAW::Model
{
class AudioGraphOutputDeviceListModel : public IAudioGraphDeviceListModel
{
    Q_OBJECT
public:
    AudioGraphOutputDeviceListModel(const YADAW::Audio::Backend::AudioGraphBackend& backend,
        QObject* parent = nullptr);
    ~AudioGraphOutputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const YADAW::Audio::Backend::AudioGraphBackend* backend_;
};
}

#endif //YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL