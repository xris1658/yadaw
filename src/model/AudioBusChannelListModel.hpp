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
    AudioBusChannelListModel(const IAudioBusConfiguration& configuration,
        bool isInput, std::uint32_t index, QObject* parent = nullptr);
    AudioBusChannelListModel(const AudioBusChannelListModel& rhs);
    ~AudioBusChannelListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
private:
    const IAudioBusConfiguration* configuration_;
    bool isInput_;
    std::uint32_t index_;
};
}

#endif //YADAW_SRC_MODEL_AUDIOBUSCHANNELLISTMODEL