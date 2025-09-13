#ifndef YADAW_SRC_MODEL_AUXINPUTSOURCELISTMODEL
#define YADAW_SRC_MODEL_AUXINPUTSOURCELISTMODEL

#include "model/AuxIOTargetListModel.hpp"

namespace YADAW::Model
{
class AuxInputSourceListModel: public AuxIOTargetListModel
{
public:
    AuxInputSourceListModel(
        YADAW::Audio::Mixer::Mixer& mixer,
        YADAW::Audio::Mixer::Mixer::ChannelListType channelListType,
        std::uint32_t channelIndex,
        bool isInstrument, bool isPreFaderInsert,
        std::uint32_t insertIndex, QObject* parent = nullptr);
    ~AuxInputSourceListModel();
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    std::vector<YADAW::Entity::IAudioIOPosition*> sources_;
};
}

#endif // YADAW_SRC_MODEL_AUXINPUTSOURCELISTMODEL