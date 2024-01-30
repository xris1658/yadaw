#ifndef YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL

#include "model/IMixerChannelListModel.hpp"

#include "audio/mixer/Mixer.hpp"

namespace YADAW::Model
{
class MixerChannelListModel: public IMixerChannelListModel
{
    Q_OBJECT
public:
    enum ListType
    {
        AudioHardwareInput,
        Regular,
        AudioHardwareOutput
    };
public:
    MixerChannelListModel(YADAW::Audio::Mixer::Mixer& mixer,
        ListType listType,
        QObject* parent = nullptr);
    ~MixerChannelListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    YADAW::Audio::Mixer::Mixer& mixer_;
    ListType listType_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
