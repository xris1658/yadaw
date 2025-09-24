#ifndef YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONLISTMODEL
#define YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONLISTMODEL

#include "model/AuxIOTargetListModel.hpp"
#include "model/AuxOutputDestinationModel.hpp"

#include <memory>

namespace YADAW::Model
{
class AuxOutputDestinationListModel: public AuxIOTargetListModel
{
    friend class AuxOutputDestinationModel;
public:
    AuxOutputDestinationListModel(
        YADAW::Audio::Mixer::Mixer& mixer,
        YADAW::Audio::Mixer::Mixer::ChannelListType channelListType,
        std::uint32_t channelIndex,
        bool isInstrument, bool isPreFaderInsert,
        std::uint32_t insertIndex, QObject* parent = nullptr);
    ~AuxOutputDestinationListModel();
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    std::vector<std::unique_ptr<AuxOutputDestinationModel>> destinations_;
};
}

#endif // YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONLISTMODEL