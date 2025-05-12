#ifndef YADAW_SRC_MODEL_SPEAKERLISTMODEL
#define YADAW_SRC_MODEL_SPEAKERLISTMODEL

#include "audio/device/IAudioDevice.hpp"
#include "model/ISpeakerListModel.hpp"

namespace YADAW::Model
{
class SpeakerListModel: public ISpeakerListModel
{
    Q_OBJECT
public:
    SpeakerListModel();
    SpeakerListModel(const YADAW::Audio::Device::IAudioChannelGroup& group);
    SpeakerListModel(const SpeakerListModel& rhs);
    SpeakerListModel& operator=(const SpeakerListModel& rhs);
    ~SpeakerListModel();
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const YADAW::Audio::Device::IAudioChannelGroup* group_ = nullptr;
};
}

#endif // YADAW_SRC_MODEL_SPEAKERLISTMODEL
