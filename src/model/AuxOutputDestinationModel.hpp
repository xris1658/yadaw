#ifndef YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL
#define YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL

#include "model/IAuxOutputDestinationListModel.hpp"

namespace YADAW::Model
{
class AuxOutputDestinationListModel;

class AuxOutputDestinationModel: public IAuxOutputDestinationListModel
{
    Q_OBJECT
public:
    AuxOutputDestinationModel(
        YADAW::Model::AuxOutputDestinationListModel& model,
        std::uint32_t channelGroupIndex, QObject* parent = nullptr);
    ~AuxOutputDestinationModel();
public:
    YADAW::Model::AuxOutputDestinationListModel& getModel() const;
    std::uint32_t getChannelGroupIndex() const;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool append(YADAW::Entity::IAudioIOPosition* position) override;
    bool remove(int position, int removeCount) override;
signals:
    void destinationAboutToBeChanged(int first, int last);
private:
    YADAW::Model::AuxOutputDestinationListModel* model_;
    std::uint32_t channelGroupIndex_;
    std::vector<YADAW::Entity::IAudioIOPosition*> positions_;
};
}

#endif // YADAW_SRC_MODEL_AUXOUTPUTDESTINATIONMODEL