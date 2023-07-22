#include "CLAPAudioChannelGroup.hpp"

namespace YADAW::Audio::Plugin
{
CLAPAudioChannelGroup::CLAPAudioChannelGroup() {}

CLAPAudioChannelGroup::CLAPAudioChannelGroup(const clap_plugin* plugin,
    const clap_plugin_audio_ports* audioPorts, bool isInput, int index)
{
    assert(audioPorts->get(plugin, index, isInput, &audioPortInfo_));
}

QString CLAPAudioChannelGroup::name() const
{
    return QString::fromUtf8(audioPortInfo_.name);
}

std::uint32_t CLAPAudioChannelGroup::channelCount() const
{
    return audioPortInfo_.channel_count;
}

YADAW::Audio::Base::ChannelGroupType CLAPAudioChannelGroup::type() const
{
    return
        std::strcmp(audioPortInfo_.port_type, CLAP_PORT_STEREO) == 0? YADAW::Audio::Base::ChannelGroupType::eStereo:
        std::strcmp(audioPortInfo_.port_type, CLAP_PORT_MONO) == 0? YADAW::Audio::Base::ChannelGroupType::eMono:
        channelCount() == 0? YADAW::Audio::Base::ChannelGroupType::eEmpty:
        YADAW::Audio::Base::ChannelGroupType::eCustomGroup;
}

YADAW::Audio::Base::ChannelType CLAPAudioChannelGroup::speakerAt(std::uint32_t index) const
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

QString CLAPAudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return QString();
}

bool CLAPAudioChannelGroup::isMain() const
{
    return audioPortInfo_.flags &CLAP_AUDIO_PORT_IS_MAIN;
}
}