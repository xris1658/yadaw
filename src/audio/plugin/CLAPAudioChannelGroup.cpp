#include "CLAPAudioChannelGroup.hpp"

#include "audio/util/CLAPHelper.hpp"
#include "util/Base.hpp"

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Base;
using namespace YADAW::Util;

using CLAPSurround = decltype(CLAP_SURROUND_FL);

ChannelType channelTypeMappingToCLAP[] = {
    ChannelType::Left,
    ChannelType::Right,
    ChannelType::Center,
    ChannelType::LFE,
    ChannelType::RearLeft,
    ChannelType::RearRight,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::RearCenter,
    ChannelType::SideLeft,
    ChannelType::SideRight,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::Custom,
    ChannelType::Custom
};

template<ChannelGroupType C>
std::uint8_t clapSpeakers[] = {};

template<> std::uint8_t clapSpeakers<ChannelGroupType::eMono>[] = {
    CLAP_SURROUND_FC
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::eStereo>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::eLRC>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_FC
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::eQuad>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_BL,
    CLAP_SURROUND_BR
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::e50>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_FC,
    CLAP_SURROUND_BL,
    CLAP_SURROUND_BR
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::e51>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_FC,
    CLAP_SURROUND_BL,
    CLAP_SURROUND_BR,
    CLAP_SURROUND_LFE
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::e61>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_FC,
    CLAP_SURROUND_BL,
    CLAP_SURROUND_BR,
    CLAP_SURROUND_BC,
    CLAP_SURROUND_LFE
};

template<> std::uint8_t clapSpeakers<ChannelGroupType::e71>[] = {
    CLAP_SURROUND_FL,
    CLAP_SURROUND_FR,
    CLAP_SURROUND_FC,
    CLAP_SURROUND_BL,
    CLAP_SURROUND_BR,
    CLAP_SURROUND_SL,
    CLAP_SURROUND_SR,
    CLAP_SURROUND_LFE
};

template<ChannelGroupType C>
std::basic_string_view<std::uint8_t> clapSpeakerView(clapSpeakers<C>, stackArraySize(clapSpeakers<C>));

template<ChannelGroupType C>
bool mappingIsChannelGroupType(const std::vector<std::uint8_t>& mapping)
{
    return
        mapping.size() == stackArraySize(clapSpeakers<C>)
        && std::for_each(mapping.begin(), mapping.end(),
            [](std::uint8_t speaker)
            {
                return std::find(
                    clapSpeakerView<C>.begin(), clapSpeakerView<C>.end(), speaker
                ) != clapSpeakerView<C>.end();
            }
        );
}

CLAPAudioChannelGroup::CLAPAudioChannelGroup() {}

CLAPAudioChannelGroup::CLAPAudioChannelGroup(const clap_plugin* plugin,
    const clap_plugin_audio_ports* audioPorts, bool isInput, std::uint32_t index)
{
    assert(audioPorts->get(plugin, index, isInput, &audioPortInfo_));
    if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_STEREO) == 0)
    {
        mapping_ = {CLAP_SURROUND_FL, CLAP_SURROUND_FR};
    }
    else if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_MONO) == 0)
    {
        mapping_ = {CLAP_SURROUND_FC};
    }
    else if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_SURROUND) == 0)
    {
        const clap_plugin_surround* surround = nullptr;
        getExtension(plugin, CLAP_EXT_SURROUND, &surround);
        assert(surround);
        mapping_.resize(audioPortInfo_.channel_count, std::numeric_limits<std::uint8_t>::max());
        auto storedCount = surround->get_channel_map(plugin, isInput, index, mapping_.data(), mapping_.size());
        assert(storedCount == mapping_.size());
        isSurround_ = true;
    }
    else if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_AMBISONIC) == 0)
    {
        const clap_plugin_ambisonic* ambisonic = nullptr;
        getExtension(plugin, CLAP_EXT_AMBISONIC, &ambisonic);
        assert(ambisonic);
        mapping_.resize(audioPortInfo_.channel_count, std::numeric_limits<std::uint8_t>::max());
        isAmbisonic_ = true;
    }
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
    if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_STEREO) == 0)
    {
        return YADAW::Audio::Base::ChannelGroupType::eStereo;
    }
    if(std::strcmp(audioPortInfo_.port_type, CLAP_PORT_MONO) == 0)
    {
        return YADAW::Audio::Base::ChannelGroupType::eMono;
    }
    if(isSurround_)
    {
        if(constexpr auto C = ChannelGroupType::eMono; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::eStereo; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::eLRC; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::eQuad; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::e50; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::e51; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::e61; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        if(constexpr auto C = ChannelGroupType::e71; mappingIsChannelGroupType<C>(mapping_))
        {
            return C;
        }
        return ChannelGroupType::eCustomGroup;
    }
    if(isAmbisonic_)
    {
        return YADAW::Audio::Base::ChannelGroupType::eCustomGroup; // TODO
    }
    if(channelCount() == 0)
    {
        return YADAW::Audio::Base::ChannelGroupType::eEmpty;
    }
    return YADAW::Audio::Base::ChannelGroupType::eCustomGroup;
}

YADAW::Audio::Base::ChannelType CLAPAudioChannelGroup::speakerAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        auto type = mapping_[index];
        return channelTypeMappingToCLAP[type];
    }
    return YADAW::Audio::Base::ChannelType::Invalid;
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