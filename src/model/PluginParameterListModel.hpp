#ifndef YADAW_SRC_MODEL_PLUGINPARAMETERLISTMODEL
#define YADAW_SRC_MODEL_PLUGINPARAMETERLISTMODEL

#include "model/IPluginParameterListModel.hpp"

#include "../audio/device/IAudioDeviceParameter.hpp"
#include "model/ParameterValueAndTextListModel.hpp"

#include <vector>

namespace YADAW::Model
{
class PluginParameterListModel: public IPluginParameterListModel
{
    Q_OBJECT
public:
    PluginParameterListModel(
        YADAW::Audio::Device::IAudioDeviceParameter& pluginParameter,
        QObject* parent = nullptr);
    ~PluginParameterListModel() override;
public:
    int itemCount() const;
    const YADAW::Audio::Device::IAudioDeviceParameter& parameter() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    QString stringFromValue(int index, double value) const override;
    double valueFromString(int index, const QString& string) const override;
private:
    YADAW::Audio::Device::IAudioDeviceParameter* pluginParameter_;
    std::vector<std::unique_ptr<ParameterValueAndTextListModel>> valueAndTextList_;
};
}

#endif // YADAW_SRC_MODEL_PLUGINPARAMETERLISTMODEL
