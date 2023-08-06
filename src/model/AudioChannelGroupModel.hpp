#ifndef YADAW_SRC_MODEL_AUDIOCHANNELGROUPMODEL
#define YADAW_SRC_MODEL_AUDIOCHANNELGROUPMODEL

#include "audio/device/IAudioChannelGroup.hpp"
#include "model/IAudioChannelGroupModel.hpp"

namespace YADAW::Model
{
class AudioChannelGroupModel: public IAudioChannelGroupModel
{
    Q_OBJECT
public:
    AudioChannelGroupModel(const YADAW::Audio::Device::IAudioChannelGroup& group,
        QObject* parent = nullptr);
    ~AudioChannelGroupModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const YADAW::Audio::Device::IAudioChannelGroup* group_;
};
}

#endif // YADAW_SRC_MODEL_AUDIOCHANNELGROUPMODEL
