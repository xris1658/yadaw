#ifndef YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL

#include "model/IMixerChannelListModel.hpp"

#include "audio/mixer/Mixer.hpp"
#include "entity/IAudioIOPosition.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/PolarityInverterModel.hpp"

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
    bool insert(int position, int count, ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCountInGroup = 0) override;
    bool append(int count, ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCountInGroup = 0) override;
    bool remove(int position, int removeCount) override;
    bool move(int position, int moveCount, int newPosition) override;
    bool copy(int position, int copyCount, int newPosition) override;
    void clear() override;
    bool setInstrument(int position, int pluginId) override;
    bool removeInstrument(int position) override;
    int getIndexOfId(const QString& id) const override;
    bool hasMute() const override;
    void unmuteAll() override;
public:
    bool isComparable(int roleIndex) const override;
    bool isFilterable(int roleIndex) const override;
    bool isSearchable(int roleIndex) const override;
    bool isLess(int roleIndex,
        const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isSearchPassed(int role, const QModelIndex& modelIndex,
        const QString& string,
        Qt::CaseSensitivity caseSensitivity) const override;
    bool isPassed(const QModelIndex& modelIndex, int role,
        const QVariant& variant) const override;
    ListType type() const;
public:
    void instrumentLatencyUpdated(std::uint32_t index) const;
    void updateInstrumentIOConfig(std::uint32_t index);
private:
    void updateInstrumentConnections(std::uint32_t from);
private:
    YADAW::Audio::Mixer::Mixer& mixer_;
    std::vector<std::unique_ptr<YADAW::Model::MixerChannelInsertListModel>> insertModels_;
    // Only used if listType_ == ListType::Regular: {
    std::vector<std::unique_ptr<InstrumentInstance>> instruments_;
    mutable std::vector<std::unique_ptr<YADAW::Model::AudioDeviceIOGroupListModel>> instrumentAudioInputs_;
    mutable std::vector<std::unique_ptr<YADAW::Model::AudioDeviceIOGroupListModel>> instrumentAudioOutputs_;
    std::vector<YADAW::Entity::IAudioIOPosition*> mainInput_;
    std::vector<YADAW::Entity::IAudioIOPosition*> mainOutput_;
    std::vector<bool> instrumentBypassed_;
    // }
    std::vector<bool> editingVolume_;
    mutable std::vector<std::unique_ptr<YADAW::Model::PolarityInverterModel>> polarityInverterModels_;
    ListType listType_;
    std::map<YADAW::Entity::IAudioIOPosition*, std::pair<std::uint32_t, QMetaObject::Connection>> connectToInputPositions_;
    std::map<YADAW::Entity::IAudioIOPosition*, std::pair<std::uint32_t, QMetaObject::Connection>> connectToOutputPositions_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELLISTMODEL
