#ifndef YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL
#define YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL

#include "model/IParameterValueAndTextListModel.hpp"

#include "audio/plugin/IParameter.hpp"

namespace YADAW::Model
{
class ParameterValueAndTextListModel: public IParameterValueAndTextListModel
{
    Q_OBJECT
public:
    // ParameterValueAndTextListModel()
private:
    const YADAW::Audio::Plugin::IParameter* parameter_;
};
}

#endif // YADAW_SRC_MODEL_PARAMETERVALUEANDTEXTLISTMODEL
