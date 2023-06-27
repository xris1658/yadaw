#ifndef YADAW_SRC_AUDIO_UTIL_AUDIOCHANNELGROUP
#define YADAW_SRC_AUDIO_UTIL_AUDIOCHANNELGROUP

#include "audio/device/IAudioChannelGroup.hpp"

namespace YADAW::Audio::Util
{
class AudioChannelGroup: public YADAW::Audio::Device::IAudioChannelGroup
{
public:
    AudioChannelGroup();
    AudioChannelGroup(const AudioChannelGroup& rhs) = default;
    AudioChannelGroup(AudioChannelGroup&& rhs) noexcept = default;
    AudioChannelGroup& operator=(const AudioChannelGroup& rhs) = default;
    AudioChannelGroup& operator=(AudioChannelGroup&& rhs) = default;
    ~AudioChannelGroup() noexcept = default;
public:
    QString name() const override;
    std::uint32_t channelCount() const override;
    YADAW::Audio::Base::ChannelGroupType type() const override;
    YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const override;
    QString speakerNameAt(std::uint32_t index) const override;
    bool isMain() const override;
public:
    void setName(const QString& name);
    void setName(QString&& name);
    void setMain(bool isMain);
    void setChannelGroupType(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0);
    bool setSpeakerType(std::uint32_t index, YADAW::Audio::Base::ChannelType type);
    bool setSpeakerName(std::uint32_t index, const QString& name);
    bool setSpeakerName(std::uint32_t index, QString&& name);
private:
    QString name_;
    YADAW::Audio::Base::ChannelGroupType type_ = Base::ChannelGroupType::NoChannel;
    bool isMain_ = false;
    std::vector<YADAW::Audio::Base::ChannelType> speakers_;
    std::vector<QString> speakerNames_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_AUDIOCHANNELGROUP
