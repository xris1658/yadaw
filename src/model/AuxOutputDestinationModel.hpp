#ifndef YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL
#define YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL

#include "model/IAuxOutputDestinationListModel.hpp"

#include "model/AuxOutputDestinationListModel.hpp"

namespace YADAW::Model
{
class AuxOutputDestinationModel: public IAuxOutputDestinationListModel
{
public:
    AuxOutputDestinationModel(
        YADAW::Model::AuxOutputDestinationListModel& model,
        std::uint32_t channelGroupIndex, QObject* parent = nullptr);
    ~AuxOutputDestinationModel();
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool append(YADAW::Entity::IAudioIOPosition* position) override;
    bool remove(int position, int removeCount) override;
private:
    YADAW::Model::AuxOutputDestinationListModel* model_;
    std::uint32_t channelGroupIndex_;
    std::vector<YADAW::Entity::IAudioIOPosition*> positions_;
};
}

#endif // YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL