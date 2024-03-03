#ifndef YADAW_SRC_MODEL_AUDIOBUSCHANNELLISTMODEL
#define YADAW_SRC_MODEL_AUDIOBUSCHANNELLISTMODEL

#include "audio/device/IAudioBusConfiguration.hpp"
#include "model/IAudioBusChannelListModel.hpp"

namespace YADAW::Model
{
using YADAW::Audio::Device::IAudioBusConfiguration;

class AudioBusConfigurationModel;

class AudioBusChannelListModel: public IAudioBusChannelListModel
{
    Q_OBJECT
public:
    AudioBusChannelListModel(IAudioBusConfiguration& configuration,
        AudioBusConfigurationModel& configurationModel,
        bool isInput, std::uint32_t index, QObject* parent = nullptr);
    AudioBusChannelListModel(const AudioBusChannelListModel& rhs);
    AudioBusChannelListModel& operator=(const AudioBusChannelListModel& rhs);
    ~AudioBusChannelListModel() override;
public:
    std::uint32_t itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
public:
    bool setChannel(std::uint32_t index, std::uint32_t deviceIndex, std::uint32_t channelIndex);
private:
    IAudioBusConfiguration* configuration_;
    AudioBusConfigurationModel* configurationModel_;
    YADAW::Audio::Device::IBus* bus_;
};
}

#endif // YADAW_SRC_MODEL_AUDIOBUSCHANNELLISTMODEL
