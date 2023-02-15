#ifndef YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP
#define YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP

#include "audio/device/IChannelGroup.hpp"

#include <pluginterfaces/vst/vstspeaker.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstcomponent.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Device;
using namespace Steinberg::Vst;

class VST3Plugin;

class VST3ChannelGroup: public IChannelGroup
{
    friend VST3Plugin;
public:
    VST3ChannelGroup();
    VST3ChannelGroup(SpeakerArrangement speakerArrangement, BusInfo busInfo);
    VST3ChannelGroup(const VST3ChannelGroup&) = default;
    VST3ChannelGroup(VST3ChannelGroup&&) noexcept = default;
    VST3ChannelGroup& operator=(VST3ChannelGroup);
    ~VST3ChannelGroup() noexcept = default;
public:
    QString name() const override;
    std::uint8_t channelCount() const override;
    YADAW::Audio::Base::ChannelGroupType type() const override;
    YADAW::Audio::Base::ChannelType speakerAt(std::uint8_t index) const override;
    QString speakerNameAt(std::uint8_t index) const override;
    bool isMain() const override;
private:
    SpeakerArrangement speakerArrangement_;
    BusInfo busInfo_;
};
}

#endif //YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP
