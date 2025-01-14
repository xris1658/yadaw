#ifndef YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL

#include "model/IMixerChannelSendListModel.hpp"

#include "audio/mixer/Mixer.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Model
{
class MixerChannelSendListModel: public IMixerChannelSendListModel
{
    Q_OBJECT
public:
    MixerChannelSendListModel(
        YADAW::Audio::Mixer::Mixer& mixer,
        YADAW::Model::MixerChannelListModel::ListType type,
        std::uint32_t channelIndex
    );
    ~MixerChannelSendListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position) override;
    bool remove(int position, int removeCount) override;
public:
    void setChannelIndex(std::uint32_t channelIndex);
private:
    YADAW::Audio::Mixer::Mixer* mixer_;
    YADAW::Model::MixerChannelListModel::ListType listType_;
    std::vector<bool> editingVolume_;
    std::vector<YADAW::Entity::IAudioIOPosition*> destinations_;
    std::vector<std::unique_ptr<YADAW::Model::PolarityInverterModel>> polarityInverterModels_;
    std::uint32_t channelIndex_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL