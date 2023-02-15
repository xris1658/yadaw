#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP
#define YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP

#include "audio/device/IChannelGroup.hpp"

#include <clap/ext/audio-ports.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Device;

class CLAPPlugin;

class CLAPChannelGroup: public IChannelGroup
{
    friend CLAPPlugin;
public:
    CLAPChannelGroup();
    CLAPChannelGroup(const clap_plugin* plugin, const clap_plugin_audio_ports* audioPorts, bool isInput, int index);
    CLAPChannelGroup(const CLAPChannelGroup&) = default;
    CLAPChannelGroup(CLAPChannelGroup&&) noexcept = default;
    ~CLAPChannelGroup() noexcept = default;
public:
    QString name() const override;
    std::uint8_t channelCount() const override;
    YADAW::Audio::Base::ChannelGroupType type() const override;
    YADAW::Audio::Base::ChannelType speakerAt(std::uint8_t index) const override;
    QString speakerNameAt(std::uint8_t index) const override;
    bool isMain() const override;
private:
    clap_audio_port_info audioPortInfo_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_CLAPCHANNELGROUP
