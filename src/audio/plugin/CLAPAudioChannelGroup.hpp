#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP
#define YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP

#include "audio/device/IAudioChannelGroup.hpp"

#include <clap/ext/audio-ports.h>
#include <clap/ext/ambisonic.h>
#include <clap/ext/surround.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Device;

class CLAPPlugin;

class CLAPAudioChannelGroup: public IAudioChannelGroup
{
    friend CLAPPlugin;
public:
    CLAPAudioChannelGroup();
    CLAPAudioChannelGroup(const clap_plugin* plugin, const clap_plugin_audio_ports* audioPorts, bool isInput, std::uint32_t index);
    CLAPAudioChannelGroup(const CLAPAudioChannelGroup&) = default;
    CLAPAudioChannelGroup(CLAPAudioChannelGroup&&) noexcept = default;
    ~CLAPAudioChannelGroup() noexcept = default;
public:
    QString name() const override;
    std::uint32_t channelCount() const override;
    YADAW::Audio::Base::ChannelGroupType type() const override;
    YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const override;
    QString speakerNameAt(std::uint32_t index) const override;
    bool isMain() const override;
private:
    bool isAmbisonic_ = false;
    bool isSurround_ = false;
    clap_audio_port_info audioPortInfo_;
    std::vector<std::uint8_t> mapping_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP
