#ifndef YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL

#include "model/IMixerChannelSendListModel.hpp"

#include "audio/mixer/Mixer.hpp"
#include "entity/SendPosition.hpp"
#include "model/PolarityInverterModel.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Model
{
class MixerChannelSendListModel: public IMixerChannelSendListModel
{
    Q_OBJECT
public:
    MixerChannelSendListModel(
        YADAW::Audio::Mixer::Mixer& mixer,
        YADAW::Audio::Mixer::Mixer::ChannelListType type,
        std::uint32_t channelIndex
    );
    ~MixerChannelSendListModel() override;
public:
    int itemCount() const;
    OptionalRef<const YADAW::Entity::SendPosition> positionAt(std::uint32_t index) const;
    OptionalRef<      YADAW::Entity::SendPosition> positionAt(std::uint32_t index);
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position) override;
    bool remove(int position, int removeCount) override;
public:
    const YADAW::Audio::Mixer::Mixer& mixer() const;
          YADAW::Audio::Mixer::Mixer& mixer();
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType() const;
    std::uint32_t channelIndex() const;
    void setChannelIndex(std::uint32_t channelIndex);
signals:
    void appended();
    void destinationChanged(int index);
    void removed(int first, int last);
private slots:
    void onAppended();
    void onDestinationChanged(int index);
    void onRemoved(int first, int last);
private:
    YADAW::Audio::Mixer::Mixer* mixer_;
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType_;
    std::vector<bool> editingVolume_;
    std::vector<YADAW::Entity::IAudioIOPosition*> destinations_;
    std::vector<std::unique_ptr<YADAW::Model::PolarityInverterModel>> polarityInverterModels_;
    std::vector<std::unique_ptr<YADAW::Entity::SendPosition>> sendPositions_;
    std::uint32_t channelIndex_;
    YADAW::Entity::IAudioIOPosition* position_ = nullptr;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELSENDLISTMODEL