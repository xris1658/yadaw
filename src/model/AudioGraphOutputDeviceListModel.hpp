#ifndef YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL
#define YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL

#if _WIN32

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/IAudioDeviceListModel.hpp"

namespace YADAW::Model
{
class AudioGraphOutputDeviceListModel : public IAudioDeviceListModel
{
    Q_OBJECT
public:
    AudioGraphOutputDeviceListModel(YADAW::Audio::Backend::AudioGraphBackend& backend,
        QObject* parent = nullptr);
    ~AudioGraphOutputDeviceListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    bool setOutputDeviceIndex(std::uint32_t index);
private:
    YADAW::Audio::Backend::AudioGraphBackend* backend_;
};
}

#endif

#endif // YADAW_SRC_MODEL_AUDIOGRAPHOUTPUTDEVICELISTMODEL