#ifndef YADAW_SRC_MODEL_AUTOMATIONMODEL
#define YADAW_SRC_MODEL_AUTOMATIONMODEL

#include "audio/base/Automation.hpp"
#include "model/IAutomationModel.hpp"

namespace YADAW::Model
{
class AutomationModel: public YADAW::Model::IAutomationModel
{
    Q_OBJECT
    Q_PROPERTY(double minValue READ getMinValue CONSTANT)
    Q_PROPERTY(double maxValue READ getMaxValue CONSTANT)
public:
    explicit AutomationModel(YADAW::Audio::Base::Automation& automation,
        QObject* parent = nullptr);
    ~AutomationModel() override;
public:
    std::size_t pointCount() const;
    double getMinValue() const;
    double getMaxValue() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    YADAW::Audio::Base::Automation* automation_;
};
}

#endif // YADAW_SRC_MODEL_AUTOMATIONMODEL
