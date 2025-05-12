#ifndef YADAW_SRC_MODEL_AUDIODEVICEIOGROUPLISTMODEL
#define YADAW_SRC_MODEL_AUDIODEVICEIOGROUPLISTMODEL

#include "model/IAudioDeviceIOGroupListModel.hpp"

#include "audio/device/IAudioDevice.hpp"
#include "model/SpeakerListModel.hpp"

namespace YADAW::Model
{
class AudioDeviceIOGroupListModel: public IAudioDeviceIOGroupListModel
{
    Q_OBJECT
public:
    AudioDeviceIOGroupListModel(
        const YADAW::Audio::Device::IAudioDevice& audioDevice,
        bool isInput,
        QObject* parent = nullptr);
    AudioDeviceIOGroupListModel(const AudioDeviceIOGroupListModel& rhs);
    AudioDeviceIOGroupListModel& operator=(const AudioDeviceIOGroupListModel& rhs);
    ~AudioDeviceIOGroupListModel();
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    void reset();
private:
    void init();
private:
    const YADAW::Audio::Device::IAudioDevice* device_ = nullptr;
    mutable std::vector<YADAW::Model::SpeakerListModel> speakerListModel_;
    bool isInput_;
};
}

#endif // YADAW_SRC_MODEL_AUDIODEVICEIOGROUPLISTMODEL
