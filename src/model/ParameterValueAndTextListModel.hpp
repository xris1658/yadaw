#ifndef YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL
#define YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL

#include "model/IParameterValueAndTextListModel.hpp"

#include "../audio/device/IParameter.hpp"

namespace YADAW::Model
{
class PluginParameterListModel;
class ParameterValueAndTextListModel: public IParameterValueAndTextListModel
{
    Q_OBJECT
public:
    ParameterValueAndTextListModel(YADAW::Model::PluginParameterListModel& model,
        int index,
        QObject* parent = nullptr);
    ~ParameterValueAndTextListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    YADAW::Model::PluginParameterListModel* model_;
    int index_;
};
}

#endif // YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL
