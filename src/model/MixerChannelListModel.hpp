#ifndef YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL

#include "model/IMixerChannelListModel.hpp"

#include "audio/mixer/Mixer.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"

#include <memory>
#include <vector>

namespace YADAW::Model
{
class MixerChannelInsertListModel;

struct InstrumentInstance;

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
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
public:
    bool insert(int position, ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCountInGroup = 0) override;
    bool append(ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCountInGroup = 0) override;
    bool remove(int position, int removeCount) override;
    bool move(int position, int moveCount, int newPosition) override;
    bool copy(int position, int copyCount, int newPosition) override;
    void clear() override;
    bool setInstrument(int position, int pluginId) override;
    bool removeInstrument(int position) override;
private:
    YADAW::Audio::Mixer::Mixer& mixer_;
    std::vector<std::unique_ptr<YADAW::Model::MixerChannelInsertListModel>> insertModels_;
    std::vector<std::unique_ptr<InstrumentInstance>> instruments_;
    std::vector<YADAW::Model::AudioDeviceIOGroupListModel> instrumentAudioInputs_;
    std::vector<YADAW::Model::AudioDeviceIOGroupListModel> instrumentAudioOutputs_;
    std::vector<ade::NodeHandle> blankInputNodes_;
    std::vector<bool> instrumentBypassed_;
    ListType listType_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
