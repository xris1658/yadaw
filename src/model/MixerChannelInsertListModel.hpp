#ifndef YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL

#include "model/IMixerChannelInsertListModel.hpp"

#include "audio/mixer/Inserts.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/MixerChannelListModel.hpp"
#include "model/PluginParameterListModel.hpp"
#include "ui/WindowAndConnection.hpp"

#include <QObject>

namespace YADAW::Model
{
class MixerChannelInsertListModel: public IMixerChannelInsertListModel
{
    Q_OBJECT
public:
    MixerChannelInsertListModel(YADAW::Audio::Mixer::Inserts& inserts,
        YADAW::Audio::Mixer::Mixer::ChannelListType type,
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
    std::uint32_t channelIndex() const;
    void setChannelIndex(std::uint32_t channelIndex);
    void setPreFaderInsert(bool isPreFaderInsert);
    void setInsertsIndex(std::uint32_t insertsIndex);
    void latencyUpdated(std::uint32_t index) const;
    void updateIOConfig(std::uint32_t index);
    const YADAW::Audio::Mixer::Inserts& inserts() const;
    YADAW::Audio::Mixer::Inserts& inserts();
public:
    void setFillPluginContextCallback(FillPluginContextCallback* callback);
    void resetFillPluginContextCallback();
signals:
    void channelIndexChanged();
private:
    void updateInsertConnections(std::uint32_t from);
private:
    YADAW::Audio::Mixer::Inserts* inserts_;
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType_;
    std::uint32_t channelIndex_;
    bool isPreFaderInsert_;
    std::uint32_t insertsIndex_;
    FillPluginContextCallback* fillPluginContextCallback_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
