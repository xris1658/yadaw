#ifndef YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP
#define YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP

#include "audio/device/IAudioChannelGroup.hpp"

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

class VST3AudioChannelGroup: public IAudioChannelGroup
{
    friend VST3Plugin;
public:
    VST3AudioChannelGroup();
    VST3AudioChannelGroup(SpeakerArrangement speakerArrangement, BusInfo busInfo);
    VST3AudioChannelGroup(const VST3AudioChannelGroup&) = default;
    VST3AudioChannelGroup(VST3AudioChannelGroup&&) noexcept = default;
    VST3AudioChannelGroup& operator=(VST3AudioChannelGroup);
    ~VST3AudioChannelGroup() noexcept = default;
public:
    QString name() const override;
    std::uint32_t channelCount() const override;
    YADAW::Audio::Base::ChannelGroupType type() const override;
    YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const override;
    QString speakerNameAt(std::uint32_t index) const override;
    bool isMain() const override;
private:
    SpeakerArrangement speakerArrangement_;
    BusInfo busInfo_;
};
}

#endif //YADAW_AUDIO_PLUGIN_VST3CHANNELGROUP
