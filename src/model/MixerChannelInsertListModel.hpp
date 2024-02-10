#ifndef YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
#define YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL

#include "model/IMixerChannelInsertListModel.hpp"

#include "audio/mixer/Inserts.hpp"
#include "controller/PluginPoolController.hpp"

namespace YADAW::Model
{
class MixerChannelInsertListModel: public IMixerChannelInsertListModel
{
public:
    MixerChannelInsertListModel(YADAW::Audio::Mixer::Inserts& inserts,
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
    bool replace(int pluginId) override;
    bool remove(int position, int removeCount) override;
    void clear() override;
private:
    YADAW::Audio::Mixer::Inserts* inserts_;
    std::vector<YADAW::Controller::PluginPool::iterator> poolIterators_;
};
}

#endif // YADAW_SRC_MODEL_MIXERCHANNELINSERTLISTMODEL
