#ifndef YADAW_SRC_MODEL_POLARITYINVERTERMODEL
#define YADAW_SRC_MODEL_POLARITYINVERTERMODEL

#include "audio/mixer/PolarityInverter.hpp"
#include "model/IPolarityInverterModel.hpp"

namespace YADAW::Model
{
class PolarityInverterModel: public IPolarityInverterModel
{
public:
    PolarityInverterModel(YADAW::Audio::Mixer::PolarityInverter& polarityInverter,
        QObject* parent = nullptr);
    ~PolarityInverterModel() override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool toggle(int channelIndex) override;
    void invertAll() override;
    void revertAll() override;
    void toggleAll() override;
private:
    YADAW::Audio::Mixer::PolarityInverter* polarityInverter_;
};
}

#endif // YADAW_SRC_MODEL_POLARITYINVERTERMODEL