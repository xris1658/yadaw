#ifndef YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL

#include "model/IMixerChannelInsertListModel.hpp"

#include "audio/mixer/Inserts.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "controller/PluginContextMap.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/MixerChannelListModel.hpp"
#include "model/PluginParameterListModel.hpp"
#include "ui/WindowAndConnection.hpp"

namespace YADAW::Model
{
class MixerChannelInsertListModel: public IMixerChannelInsertListModel
{
public:
    MixerChannelInsertListModel(YADAW::Audio::Mixer::Inserts& inserts,
        YADAW::Model::MixerChannelListModel::ListType type,
        std::uint32_t channelIndex,
        bool isPreFaderInsert,
        std::uint32_t insertsIndex,
        QObject* parent = nullptr);
    ~MixerChannelInsertListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
public:
    bool insert(int position, int pluginId) override;
    bool append(int pluginId) override;
    bool replace(int position, int pluginId) override;
    bool remove(int position, int removeCount) override;
    void clear() override;
public:
    void setChannelIndex(std::uint32_t channelIndex);
    void setPreFaderInsert(bool isPreFaderInsert);
    void setInsertsIndex(std::uint32_t insertsIndex);
    void latencyUpdated(std::uint32_t index) const;
    const YADAW::Audio::Mixer::Inserts& inserts() const;
    YADAW::Audio::Mixer::Inserts& inserts();
private:
    void updateInsertConnections(std::uint32_t from);
private:
    YADAW::Audio::Mixer::Inserts* inserts_;
    YADAW::Model::MixerChannelListModel::ListType type_;
    std::uint32_t channelIndex_;
    bool isPreFaderInsert_;
    std::uint32_t insertsIndex_;
    std::vector<YADAW::UI::WindowAndConnection> pluginEditors_;
    std::vector<YADAW::UI::WindowAndConnection> genericEditors_;
    std::vector<std::unique_ptr<YADAW::Model::PluginParameterListModel>> paramListModel_;
    std::vector<YADAW::Controller::LibraryPluginMap::iterator> libraryPluginIterators_;
    std::vector<YADAW::Controller::PluginContextMap::iterator> pluginContextIterators_;
    mutable std::vector<std::unique_ptr<YADAW::Model::AudioDeviceIOGroupListModel>> inputAudioChannelGroupLists_;
    mutable std::vector<std::unique_ptr<YADAW::Model::AudioDeviceIOGroupListModel>> outputAudioChannelGroupLists_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
