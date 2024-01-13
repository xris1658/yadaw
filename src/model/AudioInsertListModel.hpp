#ifndef YADAW_SRC_MODEL_AUDIOINSERTLISTMODEL
#define YADAW_SRC_MODEL_AUDIOINSERTLISTMODEL

#include "audio/mixer/Inserts.hpp"
#include "model/IAudioInsertListModel.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"

namespace YADAW::Model
{
class AudioInsertListModel: public IAudioInsertListModel
{
    Q_OBJECT
public:
    AudioInsertListModel(YADAW::Audio::Mixer::Inserts& inserts);
    ~AudioInsertListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
private:
    YADAW::Audio::Mixer::Inserts* inserts_;
    std::vector<YADAW::Model::AudioDeviceIOGroupListModel> inputs_;
    std::vector<YADAW::Model::AudioDeviceIOGroupListModel> outputs_;
};
}

#endif //YADAW_SRC_MODEL_AUDIOINSERTLISTMODEL
