#ifndef YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL
#define YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL

#include "MixerChannelListModel.hpp"
#include "model/IAudioIOPositionModel.hpp"

namespace YADAW::Model
{
class HardwareAudioIOPositionModel: public IAudioIOPositionModel
{
    Q_OBJECT
public:
    HardwareAudioIOPositionModel(
        YADAW::Model::MixerChannelListModel& audioHardwareIOModel,
        QObject* parent = nullptr);
    ~HardwareAudioIOPositionModel() override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    int findIndexByID(const QString& id) const override;
private:
    YADAW::Model::MixerChannelListModel* audioHardwareIOModel_;
};
}

#endif // YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL
