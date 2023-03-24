#include "CLAPChannelGroup.hpp"

namespace YADAW::Audio::Plugin
{
CLAPChannelGroup::CLAPChannelGroup() {}

CLAPChannelGroup::CLAPChannelGroup(const clap_plugin* plugin,
    const clap_plugin_audio_ports* audioPorts, bool isInput, int index)
{
    assert(audioPorts->get(plugin, index, isInput, &audioPortInfo_));
}

QString CLAPChannelGroup::name() const
{
    return QString::fromUtf8(audioPortInfo_.name);
}

std::uint8_t CLAPChannelGroup::channelCount() const
{
    return audioPortInfo_.channel_count;
}

YADAW::Audio::Base::ChannelGroupType CLAPChannelGroup::type() const
{
    return
        std::strcmp(audioPortInfo_.port_type, CLAP_PORT_STEREO) == 0? YADAW::Audio::Base::ChannelGroupType::Stereo:
        std::strcmp(audioPortInfo_.port_type, CLAP_PORT_MONO) == 0? YADAW::Audio::Base::ChannelGroupType::Mono:
        channelCount() == 0? YADAW::Audio::Base::ChannelGroupType::NoChannel:
        YADAW::Audio::Base::ChannelGroupType::Custom;
}

YADAW::Audio::Base::ChannelType CLAPChannelGroup::speakerAt(std::uint8_t index) const
{
    if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_STEREO) == 0)
    {
        return index == 0? YADAW::Audio::Base::ChannelType::Left:
               index == 1? YADAW::Audio::Base::ChannelType::Right:
               YADAW::Audio::Base::ChannelType::Invalid;
    }
    else if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_MONO) == 0)
    {
        return index == 0? YADAW::Audio::Base::ChannelType::Center: YADAW::Audio::Base::ChannelType::Invalid;
    }
    return index < channelCount()? YADAW::Audio::Base::ChannelType::Custom: YADAW::Audio::Base::ChannelType::Invalid;
}

QString CLAPChannelGroup::speakerNameAt(std::uint8_t index) const
{
    return QString();
}

bool CLAPChannelGroup::isMain() const
{
    return audioPortInfo_.flags &CLAP_AUDIO_PORT_IS_MAIN;
}
}