#ifndef YADAW_SRC_MODEL_AUDIOBUSCONFIGURATIONMODEL
#define YADAW_SRC_MODEL_AUDIOBUSCONFIGURATIONMODEL

#include "audio/device/IAudioBusConfiguration.hpp"
#include "model/AudioBusChannelListModel.hpp"
#include "model/IAudioBusConfigurationModel.hpp"

namespace YADAW::Model
{
using YADAW::Audio::Device::IAudioBusConfiguration;
class AudioBusConfigurationModel: public IAudioBusConfigurationModel
{
    Q_OBJECT
public:
    AudioBusConfigurationModel(IAudioBusConfiguration& configuration,
        bool isInput);
    ~AudioBusConfigurationModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
public:
    Q_INVOKABLE bool append(int channelCount) override;
    Q_INVOKABLE bool remove(int index) override;
    Q_INVOKABLE void clear() override;
private:
    IAudioBusConfiguration* configuration_;
    std::vector<YADAW::Model::AudioBusChannelListModel> channelList_;
    std::vector<QString> name_;
    bool isInput_;
};
}

#endif //YADAW_SRC_MODEL_AUDIOBUSCONFIGURATIONMODEL