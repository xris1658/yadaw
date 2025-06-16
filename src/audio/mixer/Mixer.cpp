#include "Mixer.hpp"

#include "audio/mixer/BlankGenerator.hpp"
#include "audio/util/InputSwitcher.hpp"
#include "util/Base.hpp"

#include <complex>
#include <ranges>
#include <unordered_map>

namespace YADAW::Audio::Mixer
{
namespace Impl
{
void blankConnectionUpdatedCallback(const Mixer&) {}

void blankPreInsertChannelCallback(const Mixer&, Mixer::PreInsertChannelCallbackArgs) {}
}

Mixer::Mixer():
    graph_(),
    graphWithPDC_(graph_),
    connectionUpdatedCallback_(&Impl::blankConnectionUpdatedCallback),
    preInsertAudioInputChannelCallback_(&Impl::blankPreInsertChannelCallback),
    preInsertRegularChannelCallback_(&Impl::blankPreInsertChannelCallback),
    preInsertAudioOutputChannelCallback_(&Impl::blankPreInsertChannelCallback)
{}

const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph() const
{
    return graphWithPDC_;
}

YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph()
{
    return graphWithPDC_;
}

const YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

const YADAW::Audio::Engine::Extension::NameTag& Mixer::nameTagExtension() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::NameTag>();
}

YADAW::Audio::Engine::Extension::NameTag& Mixer::nameTag()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::NameTag>();
}

std::uint32_t Mixer::audioInputChannelCount() const
{
    return audioInputFaders_.size();
}

std::uint32_t Mixer::channelCount() const
{
    return faders_.size();
}

std::uint32_t Mixer::audioOutputChannelCount() const
{
    return audioOutputFaders_.size();
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::audioInputChannelPolarityInverterAt(
    std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::audioOutputChannelPolarityInverterAt(
    std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::channelPolarityInverterAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*polarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::audioInputChannelPolarityInverterAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::audioOutputChannelPolarityInverterAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::channelPolarityInverterAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*polarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

std::optional<const std::uint32_t> Mixer::audioInputChannelSendCount(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {static_cast<std::uint32_t>(audioInputSendDestinations_[index].size())};
    }
    return std::nullopt;
}

std::optional<const std::uint32_t> Mixer::channelSendCount(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {static_cast<std::uint32_t>(sendDestinations_[index].size())};
    }
    return std::nullopt;
}

std::optional<const std::uint32_t> Mixer::audioOutputChannelSendCount(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {static_cast<std::uint32_t>(audioOutputSendDestinations_[index].size())};
    }
    return std::nullopt;
}

std::optional<const bool> Mixer::audioInputChannelSendIsPreFader(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendPolarityInverters = audioInputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioInputSendPolarityInverters.size())
        {
            auto inNode = audioInputSendPolarityInverters[sendIndex].second->inNodes().front();
            return {inNode == audioInputMutes_[channelIndex].second};
        }
    }
    return std::nullopt;
}

std::optional<const bool> Mixer::channelSendIsPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < channelCount())
    {
        const auto& sendPolarityInverters = sendPolarityInverters_[channelIndex];
        if(sendIndex < sendPolarityInverters.size())
        {
            auto inNode = sendPolarityInverters[sendIndex].second->inNodes().front();
            return {inNode == mutes_[channelIndex].second};
        }
    }
    return std::nullopt;
}

std::optional<const bool> Mixer::audioOutputChannelSendIsPreFader(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendPolarityInverters = audioOutputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioOutputSendPolarityInverters.size())
        {
            auto inNode = audioOutputSendPolarityInverters[sendIndex].second->inNodes().front();
            return {inNode == audioOutputMutes_[channelIndex].second};
        }
    }
    return std::nullopt;
}

std::optional<const Mixer::Position> Mixer::audioInputChannelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendDestinations = audioInputSendDestinations_[channelIndex];
        if(sendIndex < audioInputSendDestinations.size())
        {
            return {audioInputSendDestinations[sendIndex]};
        }
    }
    return std::nullopt;
}

std::optional<const Mixer::Position> Mixer::channelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < channelCount())
    {
        const auto& sendDestinations = sendDestinations_[channelIndex];
        if(sendIndex < sendDestinations.size())
        {
            return {sendDestinations[sendIndex]};
        }
    }
    return std::nullopt;
}

std::optional<const Mixer::Position> Mixer::audioOutputChannelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendDestinations = audioOutputSendDestinations_[channelIndex];
        if(sendIndex < audioOutputSendDestinations.size())
        {
            return {audioOutputSendDestinations[sendIndex]};
        }
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioInputChannelSendFaderAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendFaders = audioInputSendFaders_[channelIndex];
        if(sendIndex < audioInputSendFaders.size())
        {
            return {*audioInputSendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::ChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < channelCount())
    {
        const auto& sendFaders = sendFaders_[channelIndex];
        if(sendIndex < sendFaders.size())
        {
            return {*sendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioOutputChannelSendFaderAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendFaders = audioOutputSendFaders_[channelIndex];
        if(sendIndex < audioOutputSendFaders.size())
        {
            return {*audioOutputSendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioInputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioInputChannelCount())
    {
        auto& audioInputSendFaders = audioInputSendFaders_[channelIndex];
        if(sendIndex < audioInputSendFaders.size())
        {
            return {*audioInputSendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::channelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < channelCount())
    {
        auto& sendFaders = sendFaders_[channelIndex];
        if(sendIndex < sendFaders.size())
        {
            return {*sendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioOutputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioOutputChannelCount())
    {
        auto& audioOutputSendFaders = audioOutputSendFaders_[channelIndex];
        if(sendIndex < audioOutputSendFaders.size())
        {
            return {*audioOutputSendFaders[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioInputChannelSendMuteAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendMutes = audioInputSendMutes_[channelIndex];
        if(sendIndex < audioInputSendMutes.size())
        {
            return {*audioInputSendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::channelSendMuteAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < channelCount())
    {
        const auto& sendMutes = sendMutes_[channelIndex];
        if(sendIndex < sendMutes.size())
        {
            return {*sendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioOutputChannelSendMuteAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendMutes = audioOutputSendMutes_[channelIndex];
        if(sendIndex < audioOutputSendMutes.size())
        {
            return {*audioOutputSendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioInputChannelSendMuteAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioInputChannelCount())
    {
        auto& audioInputSendMutes = audioInputSendMutes_[channelIndex];
        if(sendIndex < audioInputSendMutes.size())
        {
            return {*audioInputSendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::channelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < channelCount())
    {
        auto& sendMutes = sendMutes_[channelIndex];
        if(sendIndex < sendMutes.size())
        {
            return {*sendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioOutputChannelSendMuteAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioOutputChannelCount())
    {
        auto& audioOutputSendMutes = audioOutputSendMutes_[channelIndex];
        if(sendIndex < audioOutputSendMutes.size())
        {
            return {*audioOutputSendMutes[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const PolarityInverter> Mixer::audioInputChannelSendPolarityInverterAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendPolarityInverters = audioInputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioInputSendPolarityInverters.size())
        {
            return {*audioInputSendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const PolarityInverter> Mixer::channelSendPolarityInverterAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < channelCount())
    {
        const auto& sendPolarityInverters = sendPolarityInverters_[channelIndex];
        if(sendIndex < sendPolarityInverters.size())
        {
            return {*sendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const PolarityInverter> Mixer::audioOutputChannelSendPolarityInverterAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendPolarityInverters = audioOutputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioOutputSendPolarityInverters.size())
        {
            return {*audioOutputSendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<PolarityInverter> Mixer::audioInputChannelSendPolarityInverterAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioInputChannelCount())
    {
        auto& audioInputSendPolarityInverters = audioInputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioInputSendPolarityInverters.size())
        {
            return {*audioInputSendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<PolarityInverter> Mixer::channelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < channelCount())
    {
        auto& sendPolarityInverters = sendPolarityInverters_[channelIndex];
        if(sendIndex < sendPolarityInverters.size())
        {
            return {*sendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<PolarityInverter> Mixer::audioOutputChannelSendPolarityInverterAt(
    std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(channelIndex < audioOutputChannelCount())
    {
        auto& audioOutputSendPolarityInverters = audioOutputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioOutputSendPolarityInverters.size())
        {
            return {*audioOutputSendPolarityInverters[sendIndex].first};
        }
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::audioInputChannelInfoAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::audioOutputChannelInfoAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::channelInfoAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(channelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::audioInputChannelInfoAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::audioOutputChannelInfoAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::channelInfoAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(channelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioInputVolumeFaderAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioOutputVolumeFaderAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::volumeFaderAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(faders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioInputVolumeFaderAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioOutputVolumeFaderAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::volumeFaderAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*(faders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioInputMuteAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioOutputMuteAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::muteAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(mutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioInputMuteAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioOutputMuteAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::muteAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*(mutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::audioInputMeterAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::audioOutputMeterAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::meterAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(meters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<Meter> Mixer::audioInputMeterAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<Meter> Mixer::audioOutputMeterAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<Meter> Mixer::meterAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*(meters_[index].first))};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType>
Mixer::audioInputChannelGroupTypeAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::audioOutputChannelGroupTypeAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::channelGroupTypeAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {mutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::audioInputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        auto& channelGroup = audioInputMutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::audioOutputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        auto& channelGroup = audioOutputMutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::channelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        auto& channelGroup = mutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioInputChannelIDAt(
    std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioOutputChannelIDAt(
    std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::channelIDAt(
    std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {channelId_[index]};
    }
    return std::nullopt;
}

bool compareIdAndIndexWithId(const Mixer::IDAndIndex& idAndIndex, IDGen::ID id)
{
    return idAndIndex.id < id;
}

OptionalRef<const Mixer::Position> Mixer::mainInputAt(std::uint32_t index) const
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        return {mainInput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainInputAt(std::uint32_t index, Position position)
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        // Disconnect
        auto toNode = inputDevices_[index].second;
        auto inEdges = toNode->inEdges();
        if(!inEdges.empty())
        {
            graph_.disconnect(inEdges.front());
        }
        auto ret = false;
        // Connect
        if(position.type == Position::Type::Invalid)
        {
            ret = true;
        }
        if(position.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != audioInputChannelIdAndIndex_.end() && it->id == position.id)
            {
                const auto& fromNode = audioInputPostFaderInserts_[it->index]->outNode();
                ret = graph_.connect(fromNode, toNode, 0, 1).has_value();
            }
        }
        else if(position.type == Position::Type::BusAndFXChannel)
        {
            // not implemented
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(ret)
        {
            if(batchUpdater_)
            {
                batchUpdater_->addNull();
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            mainInput_[index] = position;
        }
        return ret;
    }
    return false;
}

OptionalRef<const Mixer::Position> Mixer::mainOutputAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {mainOutput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainOutputAt(std::uint32_t index, Position position)
{
    if(index >= channelCount())
    {
        return false;
    }
    auto ret = false;
    const auto& mute = *(mutes_[index].first);
    const auto& channelGroup = mute.audioOutputGroupAt(0)->get();
    // Check if channel config of the destination position matches the source
    // Reconnect the graph
    auto& oldPosition = mainOutput_[index];
    if(oldPosition != position)
    {
        std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC> disconnectingOldMultiInput;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> disconnectingOldSumming;
        // Disconnect
        if(oldPosition.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldPosition.id,
                &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == oldPosition.id)
            {
                auto fromNode = postFaderInserts_[index]->outNode();
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                auto inEdges = oldSummingNode->inEdges();
                auto toChannel = graph_.getEdgeData(
                    *std::find_if(
                        inEdges.begin(), inEdges.end(),
                        [&fromNode](const ade::EdgeHandle& inEdge)
                        {
                            return inEdge->srcNode() == fromNode;
                        }
                    )
                ).toChannel;
                auto [newSumming, newSummingNode] = removeInputGroup(oldSummingAndNode, toChannel).value();
                graph_.disconnect(
                        oldSummingNode->outEdges().front()
                );
                graph_.connect(
                    newSummingNode,
                    audioOutputPolarityInverters_[it->index].second,
                    0, 0
                );
                disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                disconnectingOldSumming = std::move(oldSumming);
                oldSumming = std::move(newSumming);
                oldSummingNode = newSummingNode;
            }
        }
        else if(oldPosition.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldPosition.id,
                &compareIdAndIndexWithId
            );
            if(it != channelIdAndIndex_.end() && it->id == oldPosition.id)
            {
                auto oldType = channelInfo_[it->index].channelType;
                if(oldType == ChannelType::AudioBus || oldType == ChannelType::AudioFX)
                {
                    auto fromNode = postFaderInserts_[index]->outNode();
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                    auto inEdges = oldSummingNode->inEdges();
                    auto toChannel = graph_.getEdgeData(
                        *std::find_if(
                            inEdges.begin(), inEdges.end(),
                            [&fromNode](const ade::EdgeHandle& inEdge)
                            {
                                return inEdge->srcNode() == fromNode;
                            }
                        )
                    ).toChannel;
                    auto [newSumming, newSummingNode] = removeInputGroup(oldSummingAndNode, toChannel).value();
                    graph_.disconnect(
                        oldSummingNode->outEdges().front()
                    );
                    graph_.connect(
                        newSummingNode,
                        polarityInverters_[it->index].second,
                        0, 0
                    );
                    disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingOldSumming = std::move(oldSumming);
                    oldSumming = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                }
            }
        }
        else if(oldPosition.type == Position::Type::PluginAuxIO)
        {
            // not-implemented
        }
        // Connect
        std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC> disconnectingNewMultiInput;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> disconnectingNewSumming;
        if(position.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == position.id)
            {
                auto outputChannelIndex = it->index;
                const auto& destPair =
                    *audioOutputChannelGroupTypeAndChannelCountAt(outputChannelIndex);
                const auto& srcPair =
                    *channelGroupTypeAndChannelCountAt(index);
                if(srcPair == destPair)
                {
                    auto fromNode = postFaderInserts_[index]->outNode();
                    auto& oldSummingAndNode = audioOutputSummings_[outputChannelIndex];
                    auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                    auto [newSumming, newSummingNode] = appendInputGroup(
                        oldSummingAndNode
                    );
                    graph_.connect(
                        fromNode, newSummingNode, 0, newSumming->audioInputGroupCount() - 1
                    );
                    graph_.disconnect(audioOutputPolarityInverters_[it->index].second->inEdges().front());
                    graph_.connect(newSummingNode, audioOutputPolarityInverters_[it->index].second, 0, 0);
                    disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingNewSumming = std::move(oldSumming);
                    if(batchUpdater_)
                    {
                        batchUpdater_->addObject(
                            std::move(disconnectingOldSumming)
                        );
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                        disconnectingOldSumming.reset();
                        disconnectingOldMultiInput.reset();
                        disconnectingNewSumming.reset();
                        disconnectingNewMultiInput.reset();
                    }
                    oldSumming = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                    ret = true;
                }
            }
        }
        else if(position.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != channelIdAndIndex_.end() && it->id == position.id)
            {
                auto outputChannelIndex = it->index;
                auto channelType = channelInfo_[outputChannelIndex].channelType;
                if(channelType == ChannelType::AudioBus || channelType == ChannelType::AudioFX)
                {
                    const auto& destPair =
                        *channelGroupTypeAndChannelCountAt(outputChannelIndex);
                    const auto& srcPair =
                        *channelGroupTypeAndChannelCountAt(index);
                    if(srcPair == destPair)
                    {
                        auto fromNode = postFaderInserts_[index]->outNode();
                        auto& oldSummingAndNode = inputDevices_[outputChannelIndex];
                        auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                        auto [newSumming, newSummingNode] = appendInputGroup(
                            oldSummingAndNode
                        );
                        graph_.connect(
                            fromNode, newSummingNode, 0, newSumming->audioInputGroupCount() - 1
                        );
                        graph_.disconnect(polarityInverters_[it->index].second->inEdges().front());
                        graph_.connect(newSummingNode, polarityInverters_[it->index].second, 0, 0);
                        disconnectingNewSumming = std::move(oldSumming);
                        oldSumming = std::move(newSumming);
                        oldSummingNode = newSummingNode;
                        if(batchUpdater_)
                        {
                            batchUpdater_->addObject(std::move(disconnectingOldSumming));
                            batchUpdater_->addObject(std::move(disconnectingNewSumming));
                            batchUpdater_->addObject(std::move(disconnectingOldMultiInput));
                            batchUpdater_->addObject(std::move(disconnectingNewMultiInput));
                        }
                        else
                        {
                            disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                            connectionUpdatedCallback_(*this);
                            disconnectingOldSumming.reset();
                            disconnectingNewSumming.reset();
                            disconnectingOldMultiInput.reset();
                            disconnectingNewMultiInput.reset();
                        }
                        ret = true;
                    }
                }
            }
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        else if(position.type == Position::Type::Invalid)
        {
            if(batchUpdater_)
            {
                batchUpdater_->addObject(std::move(disconnectingOldMultiInput));
                batchUpdater_->addObject(std::move(disconnectingOldSumming));
            }
            else
            {
                connectionUpdatedCallback_(*this);
                disconnectingOldMultiInput.reset();
                disconnectingOldSumming.reset();
            }
            ret = true;
        }
    }
    if(ret)
    {
        oldPosition = position;
    }
    return ret;
}

std::optional<std::uint32_t> Mixer::getInputIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        audioInputChannelIdAndIndex_.begin(),
        audioInputChannelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != audioInputChannelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Mixer::getOutputIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        audioOutputChannelIdAndIndex_.begin(),
        audioOutputChannelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != audioOutputChannelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Mixer::getIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        channelIdAndIndex_.begin(),
        channelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != channelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

bool Mixer::hasMuteInAudioInputChannels() const
{
    auto range = YADAW::Util::IntegerRange(audioInputChannelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return audioInputMutes_[index].first->getMute();
        }
    );
}

bool Mixer::hasMuteInRegularChannels() const
{
    auto range = YADAW::Util::IntegerRange(channelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return mutes_[index].first->getMute();
        }
    );
}

bool Mixer::hasMuteInAudioOutputChannels() const
{
    auto range = YADAW::Util::IntegerRange(audioOutputChannelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return audioOutputMutes_[index].first->getMute();
        }
    );
}

void Mixer::unmuteAudioInputChannels()
{
    FOR_RANGE0(i, audioInputChannelCount())
    {
        audioInputMutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteRegularChannels()
{
    FOR_RANGE0(i, channelCount())
    {
        mutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteAudioOutputChannels()
{
    FOR_RANGE0(i, audioOutputChannelCount())
    {
        audioOutputMutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteAllChannels()
{
}

bool Mixer::appendAudioInputChannel(
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    return insertAudioInputChannel(audioInputChannelCount(), inNode, channelGroupIndex);
}

bool Mixer::insertAudioInputChannel(std::uint32_t position,
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    auto device = graph_.getNodeData(inNode).process.device();
    if(channelGroupIndex < device->audioOutputGroupCount()
       && position <= audioInputChannelCount())
    {
        preInsertAudioInputChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = 1U}
        );
        auto it = audioInputChannelId_.emplace(
            audioInputChannelId_.begin() + position,
            audioInputChannelIdGen_()
        );
        audioInputChannelIdAndIndex_.emplace(
            std::lower_bound(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                *it,
                [](IDAndIndex idAndIndex, IDGen::ID id)
                {
                    return idAndIndex.id < id;
                }
            ),
            *it, position
        );
        const auto& channelGroup = device->audioOutputGroupAt(channelGroupIndex)->get();
        auto meter = meterFactory_(channelGroup.type(), channelGroup.channelCount());
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = volumeFaderFactory_(channelGroup.type(), channelGroup.channelCount());
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        auto polarityInverter = std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto polarityInverterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter)
        );
        graph_.connect(inNode, polarityInverterNode, 0, 0);
        audioInputPreFaderInserts_.emplace(
            audioInputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, auxInputIdGen_, auxOutputIdGen_,
                polarityInverterNode, muteNode, channelGroupIndex, 0, batchUpdater_
            )
        );
        audioInputMutes_.emplace(
            audioInputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
        audioInputPostFaderInserts_.emplace(
            audioInputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, auxInputIdGen_, auxOutputIdGen_,
                faderNode, meterNode, 0, 0, batchUpdater_
            )
        );
        audioInputPolarityInverters_.emplace(
            audioInputPolarityInverters_.begin() + position,
            std::move(polarityInverter), polarityInverterNode
        );
        audioInputFaders_.emplace(
            audioInputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioInputMeters_.emplace(
            audioInputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
        auto& info = *audioInputChannelInfo_.emplace(audioInputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        audioInputSendMutes_.emplace(audioInputSendMutes_.begin() + position);
        audioInputSendFaders_.emplace(audioInputSendFaders_.begin() + position);
        audioInputSendPolarityInverters_.emplace(audioInputSendPolarityInverters_.begin() + position);
        audioInputSendDestinations_.emplace(audioInputSendDestinations_.begin() + position);
        return true;
    }
    return false;
}

bool Mixer::removeAudioInputChannel(
    std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < audioInputChannelCount()
            && last <= audioInputChannelCount()
    )
    {
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 5);
        FOR_RANGE(i, first, last)
        {
            auto polarityInverterNode = audioInputPolarityInverters_[i].second;
            nodesToRemove.emplace_back(polarityInverterNode);
            nodesToRemove.emplace_back(polarityInverterNode->inNodes().front());
            nodesToRemove.emplace_back(audioInputMutes_[i].second);
            nodesToRemove.emplace_back(audioInputFaders_[i].second);
            nodesToRemove.emplace_back(audioInputMeters_[i].second);
        }
        audioInputPreFaderInserts_.erase(
            audioInputPreFaderInserts_.begin() + first,
            audioInputPreFaderInserts_.begin() + last
        );
        audioInputPostFaderInserts_.erase(
            audioInputPostFaderInserts_.begin() + first,
            audioInputPostFaderInserts_.begin() + last
        );
        // Remove sends from the removing channel
        FOR_RANGE(i, first, last)
        {
            clearAudioInputChannelSends(i);
        }
        if(batchUpdater_)
        {
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    batchUpdater_->addObject(std::move(device));
                }
            }
        }
        else
        {
            std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> devicesToRemove;
            devicesToRemove.reserve(nodesToRemove.size());
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    devicesToRemove.emplace_back(std::move(device));
                }
            }
            connectionUpdatedCallback_(*this);
        }
        audioInputMutes_.erase(
            audioInputMutes_.begin() + first,
            audioInputMutes_.begin() + last
        );
        audioInputFaders_.erase(
            audioInputFaders_.begin() + first,
            audioInputFaders_.begin() + last
        );
        audioInputMeters_.erase(
            audioInputMeters_.begin() + first,
            audioInputMeters_.begin() + last
        );
        audioInputChannelInfo_.erase(
            audioInputChannelInfo_.begin() + first,
            audioInputChannelInfo_.begin() + last
        );
        audioInputPolarityInverters_.erase(
            audioInputPolarityInverters_.begin() + first,
            audioInputPolarityInverters_.begin() + last
        );
        audioInputSendPolarityInverters_.erase(
            audioInputSendPolarityInverters_.begin() + first,
            audioInputSendPolarityInverters_.begin() + last
        );
        audioInputSendMutes_.erase(
            audioInputSendMutes_.begin() + first,
            audioInputSendMutes_.begin() + last
        );
        audioInputSendFaders_.erase(
            audioInputSendFaders_.begin() + first,
            audioInputSendFaders_.begin() + last
        );
        audioInputSendDestinations_.erase(
            audioInputSendDestinations_.begin() + first,
            audioInputSendDestinations_.begin() + last
        );
        std::sort(
            audioInputChannelId_.begin() + first,
            audioInputChannelId_.begin() + last
        );
        auto it2 = audioInputChannelId_.begin() + first;
        audioInputChannelIdAndIndex_.erase(
            std::remove_if(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                [&it2](IDAndIndex idAndIndex)
                {
                    if(idAndIndex.id == *it2)
                    {
                        ++it2;
                        return true;
                    }
                    return false;
                }
            ),
            audioInputChannelIdAndIndex_.end()
        );
        audioInputChannelId_.erase(
            audioInputChannelId_.begin() + first,
            audioInputChannelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioInputChannels()
{
    removeAudioInputChannel(0, audioInputChannelCount());
}

bool Mixer::appendAudioOutputChannel(
    const ade::NodeHandle& outNode, std::uint32_t channelGroupIndex)
{
    return insertAudioOutputChannel(audioOutputChannelCount(), outNode, channelGroupIndex);
}

bool Mixer::insertAudioOutputChannel(std::uint32_t position,
    const ade::NodeHandle& outNode, std::uint32_t channel)
{
    // input device of an audio output channel is a summing.
    auto device = graph_.getNodeData(outNode).process.device();
    if(channel < device->audioInputGroupCount()
        && position <= audioOutputChannelCount())
    {
        preInsertAudioOutputChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = 1U}
        );
        auto it = audioOutputChannelId_.emplace(
            audioOutputChannelId_.begin() + position,
            audioOutputChannelIdGen_()
        );
        audioOutputChannelIdAndIndex_.emplace(
            std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                *it,
                [](IDAndIndex idAndIndex, IDGen::ID id)
                {
                    return idAndIndex.id < id;
                }
            ),
            *it, position
        );
        const auto& channelGroup = device->audioInputGroupAt(channel)->get();
        auto summing = std::make_unique<YADAW::Audio::Util::Summing>(
            0, channelGroup.type(), channelGroup.channelCount()
        );
        auto summingNode = graphWithPDC_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*summing)
        );
        auto meter = meterFactory_(channelGroup.type(), channelGroup.channelCount());
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = volumeFaderFactory_(channelGroup.type(), channelGroup.channelCount());
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        auto polarityInverter = std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto polarityInverterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter)
        );
        audioOutputPreFaderInserts_.emplace(
            audioOutputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, auxInputIdGen_, auxOutputIdGen_,
                polarityInverterNode, muteNode, 0, 0, batchUpdater_
            )
        );
        graph_.connect(summingNode, polarityInverterNode, 0, 0);
        audioOutputMutes_.emplace(
            audioOutputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
        audioOutputPostFaderInserts_.emplace(
            audioOutputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, auxInputIdGen_, auxOutputIdGen_,
                faderNode, meterNode, 0, 0, batchUpdater_
            )
        );
        graph_.connect(meterNode, outNode, 0, channel);
        audioOutputSummings_.emplace(
            audioOutputSummings_.begin() + position,
            std::move(summing), summingNode
        );
        audioOutputFaders_.emplace(
            audioOutputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioOutputMeters_.emplace(
            audioOutputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        audioOutputPolarityInverters_.emplace(
            audioOutputPolarityInverters_.begin() + position,
            std::move(polarityInverter), polarityInverterNode
        );
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
        auto& info = *audioOutputChannelInfo_.emplace(audioOutputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        audioOutputSendMutes_.emplace(audioOutputSendMutes_.begin() + position);
        audioOutputSendFaders_.emplace(audioOutputSendFaders_.begin() + position);
        audioOutputSendPolarityInverters_.emplace(audioOutputSendPolarityInverters_.begin() + position);
        audioOutputSendDestinations_.emplace(audioOutputSendDestinations_.begin() + position);
        return true;
    }
    return false;
}

bool Mixer::removeAudioOutputChannel(
    std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < audioOutputChannelCount()
            && last <= audioOutputChannelCount()
    )
    {
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 6);
        FOR_RANGE(i, first, last)
        {
            nodesToRemove.emplace_back(audioOutputSummings_[i].second);
            nodesToRemove.emplace_back(audioOutputMutes_[i].second);
            nodesToRemove.emplace_back(audioOutputFaders_[i].second);
            nodesToRemove.emplace_back(audioOutputMeters_[i].second);
            nodesToRemove.emplace_back(
                *(audioOutputMeters_[i].second->outNodes().begin())
            );
            nodesToRemove.emplace_back(
                audioOutputPolarityInverters_[i].second
            );
        }
        audioOutputPreFaderInserts_.erase(
            audioOutputPreFaderInserts_.begin() + first,
            audioOutputPreFaderInserts_.begin() + last
        );
        audioOutputPostFaderInserts_.erase(
            audioOutputPostFaderInserts_.begin() + first,
            audioOutputPostFaderInserts_.begin() + last
        );
        // Remove sends from the removing channel
        FOR_RANGE(i, first, last)
        {
            clearAudioOutputChannelSends(i);
        }
        if(batchUpdater_)
        {
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    batchUpdater_->addObject(std::move(device));
                }
            }
        }
        else
        {
            std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> devicesToRemove;
            devicesToRemove.reserve(nodesToRemove.size());
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    devicesToRemove.emplace_back(std::move(device));
                }
            }
            connectionUpdatedCallback_(*this);
        }
        audioOutputMutes_.erase(
            audioOutputMutes_.begin() + first,
            audioOutputMutes_.begin() + last
        );
        audioOutputSummings_.erase(
            audioOutputSummings_.begin() + first,
            audioOutputSummings_.begin() + last
        );
        audioOutputFaders_.erase(
            audioOutputFaders_.begin() + first,
            audioOutputFaders_.begin() + last
        );
        audioOutputMeters_.erase(
            audioOutputMeters_.begin() + first,
            audioOutputMeters_.begin() + last
        );
        audioOutputPolarityInverters_.erase(
            audioOutputPolarityInverters_.begin() + first,
            audioOutputPolarityInverters_.begin() + last
        );
        audioOutputSendPolarityInverters_.erase(
            audioOutputSendPolarityInverters_.begin() + first,
            audioOutputSendPolarityInverters_.begin() + last
        );
        audioOutputSendMutes_.erase(
            audioOutputSendMutes_.begin() + first,
            audioOutputSendMutes_.begin() + last
        );
        audioOutputSendFaders_.erase(
            audioOutputSendFaders_.begin() + first,
            audioOutputSendFaders_.begin() + last
        );
        audioOutputSendDestinations_.erase(
            audioOutputSendDestinations_.begin() + first,
            audioOutputSendDestinations_.begin() + last
        );
        audioOutputChannelInfo_.erase(
            audioOutputChannelInfo_.begin() + first,
            audioOutputChannelInfo_.begin() + last
        );
        std::sort(
            audioOutputChannelId_.begin() + first,
            audioOutputChannelId_.begin() + last
        );
        auto it2 = audioOutputChannelId_.begin() + first;
        audioOutputChannelIdAndIndex_.erase(
            std::remove_if(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                [&it2](IDAndIndex idAndIndex)
                {
                    if(idAndIndex.id == *it2)
                    {
                        ++it2;
                        return true;
                    }
                    return false;
                }
            ),
            audioOutputChannelIdAndIndex_.end()
        );
        audioOutputChannelId_.erase(
            audioOutputChannelId_.begin() + first,
            audioOutputChannelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioOutputChannels()
{
    removeAudioOutputChannel(0, audioOutputChannelCount());
}

bool Mixer::insertChannels(
    std::uint32_t position, std::uint32_t count,
    ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    std::vector<DeviceAndNode> inputDevicesAndNode;
    std::vector<FaderAndNode> fadersAndNode;
    std::vector<MeterAndNode> metersAndNode;
    std::vector<MuteAndNode> mutesAndNode;
    std::vector<PolarityInverterAndNode> polarityInvertersAndNode;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts;
    auto ret = false;
    if(position <= channelCount() && count > 0)
    {
        // inputs ----------------------------------------------------------
        inputDevicesAndNode.reserve(count);
        // polarity inverters -----------------------------------------------
        polarityInvertersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& [device, node] = polarityInvertersAndNode.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
                    channelGroupType, channelCountInGroup
                ),
                ade::NodeHandle()
            );
            node = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(*device)
            );
        }
        // faders ----------------------------------------------------------
        fadersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& faderAndNode =  fadersAndNode.emplace_back(
                volumeFaderFactory_(channelGroupType, channelCountInGroup),
                ade::NodeHandle()
            );
            faderAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(faderAndNode.first)
                )
            );
        }
        // meters ----------------------------------------------------------
        metersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& meterAndNode = metersAndNode.emplace_back(
                meterFactory_(channelGroupType, channelCountInGroup),
                ade::NodeHandle()
            );
            meterAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(meterAndNode.first)
                )
            );
        }
        // mutes -----------------------------------------------------------
        mutesAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& muteAndNode = mutesAndNode.emplace_back(
                std::make_unique<YADAW::Audio::Util::Mute>(
                    channelGroupType, channelCountInGroup
                ),
                ade::NodeHandle()
            );
            muteAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(muteAndNode.first)
                )
            );
        }
        // post-fader inserts ----------------------------------------------
        postFaderInserts.reserve(count);
        FOR_RANGE0(i, count)
        {
            postFaderInserts.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::Inserts>(
                    graph_, auxInputIdGen_, auxOutputIdGen_,
                    fadersAndNode[i].second, metersAndNode[i].second, 0, 0, batchUpdater_
                )
            );
        }
        switch(channelType)
        {
        case ChannelType::Audio:
        {
            // inputs ----------------------------------------------------------
            std::vector<std::unique_ptr<YADAW::Audio::Util::InputSwitcher>> inputSwitchers;
            inputSwitchers.reserve(count);
            FOR_RANGE0(i, count)
            {
                inputSwitchers.emplace_back(
                    std::make_unique<YADAW::Audio::Util::InputSwitcher>(
                        2, channelGroupType, channelCountInGroup
                    )
                );
            }
            FOR_RANGE0(i, count)
            {
                auto node = graphWithPDC_.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(
                        *inputSwitchers[i]
                    )
                );
                auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                    graph_.getNodeData(node).process.device()
                );
                multiInput->setBufferSize(bufferExtension().bufferSize());
                inputDevicesAndNode.emplace_back(std::move(inputSwitchers[i]), node);
            }
            ret = true;
            break;
        }
        case ChannelType::Instrument:
        {
            std::generate_n(
                std::back_inserter(inputDevicesAndNode),
                count,
                []()
                {
                    return DeviceAndNode{
                        std::unique_ptr<YADAW::Audio::Device::IAudioDevice>(),
                        ade::NodeHandle()
                    };
                }
            );
            ret = true;
            break;
        }
        case ChannelType::AudioFX:
        case ChannelType::AudioBus:
        {
            // inputs ----------------------------------------------------------
            inputDevicesAndNode.reserve(count);
            {
                std::vector<std::unique_ptr<YADAW::Audio::Util::Summing>> summings;
                summings.reserve(count);
                FOR_RANGE0(i, count)
                {
                    summings.emplace_back(
                        std::make_unique<YADAW::Audio::Util::Summing>(
                            0, channelGroupType, channelCountInGroup
                        )
                    );
                }
                FOR_RANGE0(i, count)
                {
                    auto node = graph_.addNode(
                        YADAW::Audio::Engine::AudioDeviceProcess(
                            *summings[i]
                        )
                    );
                    inputDevicesAndNode.emplace_back(summings[i].release(), node);
                }
            }
            ret = true;
            break;
        }
        }
    }
    if(ret)
    {
        preInsertRegularChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = count}
        );
        // instruments -----------------------------------------------------
        std::fill_n(
            std::inserter(
                instrumentBypassed_,
                instrumentBypassed_.begin() + position
            ),
            count, false
        );
        std::fill_n(
            std::inserter(
                instrumentOutputChannelIndex_,
                instrumentOutputChannelIndex_.begin() + position
            ),
            count, 0U
        );
        std::generate_n(
            std::inserter(
                instrumentContexts_, instrumentContexts_.begin() + position
            ),
            count,
            []()
            {
                return YADAW::Util::createUniquePtr(nullptr);
            }
        );
        instrumentAuxInputIDs_.insert(
            instrumentAuxInputIDs_.begin() + position, count,
            decltype(instrumentAuxInputIDs_)::value_type()
        );
        instrumentAuxOutputIDs_.insert(
            instrumentAuxOutputIDs_.begin() + position, count,
            decltype(instrumentAuxOutputIDs_)::value_type()
        );
        // pre-fader inserts -----------------------------------------------
        std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts;
        preFaderInserts.reserve(count);
        FOR_RANGE0(i, count)
        {
            preFaderInserts.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::Inserts>(
                    graph_, auxInputIdGen_, auxOutputIdGen_,
                    polarityInvertersAndNode[i].second, mutesAndNode[i].second,
                    0, 0, batchUpdater_
                )
            );
        }
        // connect and move-------------------------------------------------
        if(channelType != ChannelType::Instrument)
        {
            FOR_RANGE0(i, count)
            {
                graph_.connect(
                    inputDevicesAndNode[i].second, polarityInvertersAndNode[i].second, 0, 0
                );
            }
        }
        FOR_RANGE0(i, count)
        {
            graph_.connect(
                mutesAndNode[i].second, fadersAndNode[i].second, 0, 0
            );
        }
        std::move(
            inputDevicesAndNode.begin(), inputDevicesAndNode.end(),
            std::inserter(
                inputDevices_, inputDevices_.begin() + position
            )
        );
        std::move(
            polarityInvertersAndNode.begin(), polarityInvertersAndNode.end(),
            std::inserter(
                polarityInverters_, polarityInverters_.begin() + position
            )
        );
        std::move(
            preFaderInserts.begin(), preFaderInserts.end(),
            std::inserter(
                preFaderInserts_, preFaderInserts_.begin() + position
            )
        );
        std::move(
            postFaderInserts.begin(), postFaderInserts.end(),
            std::inserter(
                postFaderInserts_, postFaderInserts_.begin() + position
            )
        );
        std::move(
            metersAndNode.begin(), metersAndNode.end(),
            std::inserter(
                meters_, meters_.begin() + position
            )
        );
        std::move(
            fadersAndNode.begin(), fadersAndNode.end(),
            std::inserter(
                faders_, faders_.begin() + position
            )
        );
        std::move(
            mutesAndNode.begin(), mutesAndNode.end(),
            std::inserter(
                mutes_, mutes_.begin() + position
            )
        );
        // send
        std::generate_n(
            std::inserter(
                sendPolarityInverters_,
                sendPolarityInverters_.begin() + position), count,
            []() -> decltype(sendPolarityInverters_)::value_type
            {
                return {};
            }
        );
        std::generate_n(
            std::inserter(
                sendMutes_,
                sendMutes_.begin() + position), count,
            []() -> decltype(sendMutes_)::value_type
            {
                return {};
            }
        );
        std::generate_n(
            std::inserter(
                sendFaders_,
                sendFaders_.begin() + position), count,
            []() -> decltype(sendFaders_)::value_type
            {
                return {};
            }
        );
        std::fill_n(
            std::inserter(
                sendDestinations_,
                sendDestinations_.begin() + position), count,
                decltype(sendDestinations_)::value_type()
        );
        // info, etc.
        std::fill_n(
            std::inserter(channelInfo_, channelInfo_.begin() + position), count,
            ChannelInfo {QString(), QColor(), channelType}
        );
        std::generate_n(
            std::inserter(channelId_, channelId_.begin() + position), count,
            [this]()
            {
                return channelIdGen_();
            }
        );
        for(auto& [id, index]: channelIdAndIndex_)
        {
            if(index >= position)
            {
                index += count;
            }
        }
        FOR_RANGE0(i, count)
        {
            channelIdAndIndex_.emplace(
                std::lower_bound(
                    channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                    channelId_[position + i],
                    [](IDAndIndex idAndIndex, IDGen::ID id)
                    {
                        return idAndIndex.id < id;
                    }
                ),
                channelId_[position + i], position + i
            );
        }
        std::fill_n(
            std::inserter(mainInput_, mainInput_.begin() + position), count,
            Position {
                Position::Invalid, 0, IDGen::InvalidId
            }
        );
        std::fill_n(
            std::inserter(mainOutput_, mainOutput_.begin() + position), count,
            Position {Position::Invalid, 0, IDGen::InvalidId}
        );
        connectionUpdatedCallback_(*this);
    }
    return ret;
}

bool Mixer::appendChannels(
    std::uint32_t count, ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    return insertChannels(
        channelCount(), count, channelType, channelGroupType, channelCountInGroup);
}

bool Mixer::removeChannel(std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < channelCount() && last <= channelCount())
    {
        instrumentBypassed_.erase(
            instrumentBypassed_.begin() + first,
            instrumentBypassed_.begin() + last
        );
        instrumentOutputChannelIndex_.erase(
            instrumentOutputChannelIndex_.begin() + first,
            instrumentOutputChannelIndex_.begin() + last
        );
        instrumentAuxInputIDs_.erase(
            instrumentAuxInputIDs_.begin() + first,
            instrumentAuxInputIDs_.begin() + last
        );
        instrumentAuxOutputIDs_.erase(
            instrumentAuxOutputIDs_.begin() + first,
            instrumentAuxOutputIDs_.begin() + last
        );
        preFaderInserts_.erase(
            preFaderInserts_.begin() + first,
            preFaderInserts_.begin() + last
        );
        postFaderInserts_.erase(
            postFaderInserts_.begin() + first,
            postFaderInserts_.begin() + last
        );
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 5);
        FOR_RANGE(i, first, last)
        {
            auto nodeToRemove = inputDevices_[i].second;
            if(nodeToRemove != nullptr)
            {
                nodesToRemove.emplace_back(nodeToRemove);
            }
            nodesToRemove.emplace_back(polarityInverters_[i].second);
            nodesToRemove.emplace_back(mutes_[i].second);
            nodesToRemove.emplace_back(faders_[i].second);
            nodesToRemove.emplace_back(meters_[i].second);
        }
        // Remove sends from the removing channel
        FOR_RANGE(i, first, last)
        {
            clearChannelSends(i);
        }
        if(batchUpdater_)
        {
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    batchUpdater_->addObject(std::move(device));
                }
            }
        }
        else
        {
            std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> devicesToRemove;
            devicesToRemove.reserve(nodesToRemove.size());
            FOR_RANGE0(i, nodesToRemove.size())
            {
                if(auto device = graphWithPDC_.removeNode(nodesToRemove[i]))
                {
                    devicesToRemove.emplace_back(std::move(device));
                }
            }
            connectionUpdatedCallback_(*this);
        }
        mainOutput_.erase(
            mainOutput_.begin() + first,
            mainOutput_.begin() + last
        );
        mainInput_.erase(
            mainInput_.begin() + first,
            mainInput_.begin() + last
        );
        inputDevices_.erase(
            inputDevices_.begin() + first,
            inputDevices_.begin() + last
        );
        instrumentContexts_.erase(
            instrumentContexts_.begin() + first,
            instrumentContexts_.begin() + last
        );
        polarityInverters_.erase(
            polarityInverters_.begin() + first,
            polarityInverters_.begin() + last
        );
        mutes_.erase(
            mutes_.begin() + first,
            mutes_.begin() + last
        );
        meters_.erase(
            meters_.begin() + first,
            meters_.begin() + last
        );
        faders_.erase(
            faders_.begin() + first,
            faders_.begin() + last
        );
        sendPolarityInverters_.erase(
            sendPolarityInverters_.begin() + first,
            sendPolarityInverters_.begin() + last
        );
        sendMutes_.erase(
            sendMutes_.begin() + first,
            sendMutes_.begin() + last
        );
        sendFaders_.erase(
            sendFaders_.begin() + first,
            sendFaders_.begin() + last
        );
        sendDestinations_.erase(
            sendDestinations_.begin() + first,
            sendDestinations_.begin() + last
        );
        channelInfo_.erase(
            channelInfo_.begin() + first,
            channelInfo_.begin() + last
        );
        std::sort(
            channelId_.begin() + first,
            channelId_.begin() + last
        );
        auto it2 = channelId_.begin() + first;
        channelIdAndIndex_.erase(
            std::remove_if(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                [this, first, last](IDAndIndex idAndIndex)
                {
                    auto it = std::lower_bound(
                        channelId_.begin() + first,
                        channelId_.begin() + last,
                        idAndIndex.id
                    );
                    return it != channelId_.begin() + last
                        && *it == idAndIndex.id;
                }
            ),
            channelIdAndIndex_.end()
        );
        for(auto& [id, index]: channelIdAndIndex_)
        {
            if(index >= last)
            {
                index -= removeCount;
            }
        }
        channelId_.erase(
            channelId_.begin() + first,
            channelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearChannels()
{
    removeChannel(0, channelCount());
}

std::optional<bool> Mixer::appendAudioInputChannelSend(
    std::uint32_t channelIndex, bool isPreFader, Position destination)
{
    if(channelIndex < audioInputChannelCount())
    {
        return insertAudioInputChannelSend(channelIndex, audioInputSendDestinations_[channelIndex].size(), isPreFader, destination);
    }
    return std::nullopt;
}

std::optional<bool> Mixer::insertAudioInputChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination)
{
    if(channelIndex < audioInputChannelCount())
    {
        if(sendPosition <= audioInputSendPolarityInverters_[channelIndex].size())
        {
            if(destination.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    audioOutputChannelIdAndIndex_.begin(), audioOutputChannelIdAndIndex_.end(),
                    destination.id
                );
                if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    auto fromNode = isPreFader? audioInputMutes_[channelIndex].second: audioInputFaders_[channelIndex].second;
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                    {
                        auto newSummingAndNode = appendInputGroup(audioOutputSummings_[it->index]);
                        graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                        graph_.connect(
                            newSummingAndNode.second, audioOutputPolarityInverters_[it->index].second, 0, 0
                        );
                        auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                            fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                        );
                        audioInputSendPolarityInverters_[channelIndex].emplace(
                            audioInputSendPolarityInverters_[channelIndex].begin() + sendPosition,
                            std::move(polarityInverterAndNode)
                        );
                        audioInputSendMutes_[channelIndex].emplace(
                            audioInputSendMutes_[channelIndex].begin() + sendPosition,
                            std::move(muteAndNode)
                        );
                        audioInputSendFaders_[channelIndex].emplace(
                            audioInputSendFaders_[channelIndex].begin() + sendPosition,
                            std::move(faderAndNode)
                        );
                        audioInputSendDestinations_[channelIndex].emplace(
                            audioInputSendDestinations_[channelIndex].begin() + sendPosition,
                            destination
                        );
                        if(batchUpdater_)
                        {
                            if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                            {
                                batchUpdater_->addObject(std::move(multiInputWithPDC));
                            }
                            batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                            oldSummingAndNode.first = std::move(newSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                        }
                        else
                        {
                            auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                            std::swap(newSummingAndNode.first, oldSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            connectionUpdatedCallback_(*this);
                        }
                        return {true};
                    }
                }
            }
            else if(destination.type == Position::Type::BusAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                    destination.id
                );
                if(it != channelIdAndIndex_.end()
                    && it->id == destination.id)
                {
                    auto type = channelInfo_[it->index].channelType;
                    if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                    {
                        auto& oldSummingAndNode = inputDevices_[it->index];
                        auto fromNode = isPreFader? audioInputMutes_[channelIndex].second: audioInputFaders_[channelIndex].second;
                        if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                        {
                            auto newSummingAndNode = appendInputGroup(inputDevices_[it->index]);
                            graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                            graph_.connect(
                                newSummingAndNode.second, polarityInverters_[it->index].second, 0, 0
                            );
                            auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                                fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                            );
                            audioInputSendPolarityInverters_[channelIndex].emplace(
                                audioInputSendPolarityInverters_[channelIndex].begin() + sendPosition,
                                std::move(polarityInverterAndNode)
                            );
                            audioInputSendMutes_[channelIndex].emplace(
                                audioInputSendMutes_[channelIndex].begin() + sendPosition,
                                std::move(muteAndNode)
                            );
                            audioInputSendFaders_[channelIndex].emplace(
                                audioInputSendFaders_[channelIndex].begin() + sendPosition,
                                std::move(faderAndNode)
                            );
                            audioInputSendDestinations_[channelIndex].emplace(
                                audioInputSendDestinations_[channelIndex].begin() + sendPosition,
                                destination
                            );
                            if(batchUpdater_)
                            {
                                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                                {
                                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                                }
                                batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                                connectionUpdatedCallback_(*this);
                            }
                            else
                            {
                                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                                std::unique_ptr<YADAW::Audio::Util::Summing> oldSumming(
                                    static_cast<YADAW::Audio::Util::Summing*>(oldSummingAndNode.first.release())
                                );
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                                connectionUpdatedCallback_(*this);
                            }
                            return {true};
                        }
                    }
                }
            }
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setAudioInputChannelSendPreFader(
    std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader)
{
    if(channelIndex < audioInputChannelCount())
    {
        const auto& audioInputSendPolarityInverters = audioInputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioInputSendPolarityInverters.size())
        {
            auto inNode = audioInputSendPolarityInverters[sendIndex].second->inNodes().front();
            auto prevIsPreFader = inNode == audioInputMutes_[channelIndex].second;
            if(prevIsPreFader != preFader)
            {
                auto newInNode = preFader? audioInputMutes_[channelIndex].second: audioInputFaders_[channelIndex].second;
                graph_.disconnect(audioInputSendPolarityInverters[sendIndex].second->inEdges().front());
                graph_.connect(newInNode, audioInputSendPolarityInverters[channelIndex].second, 0, 0);
                return {true};
            }
            return {false};
        }
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setAudioInputChannelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination)
{
    if(channelIndex < audioInputChannelCount() && sendIndex < audioInputSendDestinations_[channelIndex].size())
    {
        auto oldDestination = audioInputSendDestinations_[channelIndex][sendIndex];
        ade::NodeHandle inNode;
        std::uint32_t prevChannelIndex = 0;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> oldSumming;
        ade::NodeHandle oldSummingNode;
        if(oldDestination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = audioOutputSummings_[it->index].second;
        }
        else if(oldDestination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = inputDevices_[it->index].second;
        }
        else if(oldDestination.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(oldSummingNode != nullptr)
        {
            inNode = audioInputSendPolarityInverters_[channelIndex][sendIndex].second->inNodes().front();
            for(const auto& outEdge: inNode->outEdges())
            {
                if(outEdge->dstNode() == oldSummingNode)
                {
                    prevChannelIndex = graph_.getEdgeData(outEdge).toChannel;
                    break;
                }
            }
        }
        auto connected = false;
        if(destination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                destination.id
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
            {
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                {
                    auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                    graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                    oldSumming = std::move(oldSummingAndNode.first);
                    oldSummingNode = oldSummingAndNode.second;
                    oldSummingAndNode = std::move(newSummingAndNode);
                    connected = true;
                }
            }
        }
        else if(destination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                destination.id
            );
            if(it != channelIdAndIndex_.end() && it->id == destination.id)
            {
                auto type = channelInfo_[it->index].channelType;
                if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                {
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                        oldSumming.reset(oldSummingAndNode.first.release());
                        oldSummingNode = oldSummingAndNode.second;
                        oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                        oldSummingAndNode.second = std::move(newSummingAndNode.second);
                        connected = true;
                    }
                }
            }
        }
        else if(destination.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(connected)
        {
            if(batchUpdater_)
            {
                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingNode))
                {
                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                }
            }
            else
            {
                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingNode);
                connectionUpdatedCallback_(*this);
            }
        }
        else if(oldSummingNode != nullptr)
        {
            graph_.connect(inNode, oldSummingNode, 0, prevChannelIndex);
        }
        return {connected};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::removeAudioInputChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount)
{
    if(channelIndex < audioInputChannelCount())
    {
        auto& sendDestinations = audioInputSendDestinations_[channelIndex];
        auto& sendFaders = audioInputSendFaders_[channelIndex];
        auto& sendPolarityInverters = audioInputSendPolarityInverters_[channelIndex];
        auto& sendMutes = audioInputSendMutes_[channelIndex];
        auto sendCount = sendDestinations.size();
        if(sendPosition < sendCount && sendPosition + removeCount <= sendCount)
        {
            std::vector<Position> destinations;
            destinations.reserve(removeCount);
            FOR_RANGE(i, sendPosition, sendPosition + removeCount)
            {
                if(std::find(destinations.begin(), destinations.end(), sendDestinations[i]) != destinations.end())
                {
                    destinations.emplace_back(sendDestinations[i]);
                }
                graph_.removeNode(sendFaders[i].second);
                graph_.removeNode(sendMutes[i].second);
                graph_.removeNode(sendPolarityInverters[i].second);
            }
            std::vector<SummingAndNode> removingSummings;
            removingSummings.reserve(destinations.size());
            FOR_RANGE0(i, destinations.size())
            {
                auto destination = destinations[i];
                if(destination.type == Position::Type::AudioHardwareIOChannel)
                {
                    auto it = std::lower_bound(
                        audioOutputChannelIdAndIndex_.begin(),
                        audioOutputChannelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    removingSummings.emplace_back(
                        shrinkInputGroups(oldSummingAndNode)
                    ).swap(oldSummingAndNode);
                }
                else if(destination.type == Position::Type::PluginAuxIO)
                {
                    auto it = std::lower_bound(
                        channelIdAndIndex_.begin(),
                        channelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& newSummingAndNode = removingSummings.emplace_back(shrinkInputGroups(oldSummingAndNode));
                    std::swap(oldSummingAndNode.second, newSummingAndNode.second);
                    auto ptr = oldSummingAndNode.first.release();
                    oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                }
            }
            if(batchUpdater_)
            {
                FOR_RANGE(i, sendPosition, sendPosition + removeCount)
                {
                    batchUpdater_->addObject(std::move(sendFaders[i].first));
                    batchUpdater_->addObject(std::move(sendMutes[i].first));
                    batchUpdater_->addObject(std::move(sendPolarityInverters[i].first));
                }
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            sendDestinations.erase(
                sendDestinations.begin() + sendPosition,
                sendDestinations.begin() + sendPosition + removeCount
            );
            sendFaders.erase(
                sendFaders.begin() + sendPosition,
                sendFaders.begin() + sendPosition + removeCount
            );
            sendMutes.erase(
                sendMutes.begin() + sendPosition,
                sendMutes.begin() + sendPosition + removeCount
            );
            sendPolarityInverters.erase(
                sendPolarityInverters.begin() + sendPosition,
                sendPolarityInverters.begin() + sendPosition + removeCount
            );
            return {true};
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::clearAudioInputChannelSends(std::uint32_t channelIndex)
{
    if(channelIndex < audioInputChannelCount())
    {
        return removeAudioInputChannelSend(channelIndex, 0, audioInputSendDestinations_[channelIndex].size());
    }
    return std::nullopt;
}

std::optional<bool> Mixer::appendChannelSend(std::uint32_t channelIndex, bool isPreFader, Position destination)
{
    if(channelIndex < channelCount())
    {
        return insertChannelSend(channelIndex, *channelSendCount(channelIndex), isPreFader, destination);
    }
    return std::nullopt;
}

std::optional<bool> Mixer::insertChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination)
{
    if(channelIndex < channelCount())
    {
        if(sendPosition <= sendPolarityInverters_[channelIndex].size())
        {
            if(destination.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    audioOutputChannelIdAndIndex_.begin(),
                    audioOutputChannelIdAndIndex_.end(),
                    destination.id
                );
                if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    auto fromNode = isPreFader? mutes_[channelIndex].second: faders_[channelIndex].second;
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                        graph_.connect(
                            newSummingAndNode.second, audioOutputPolarityInverters_[it->index].second, 0, 0
                        );
                        auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                            fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                        );
                        sendPolarityInverters_[channelIndex].emplace(
                            sendPolarityInverters_[channelIndex].begin() + sendPosition,
                            std::move(polarityInverterAndNode)
                        );
                        sendMutes_[channelIndex].emplace(
                            sendMutes_[channelIndex].begin() + sendPosition,
                            std::move(muteAndNode)
                        );
                        sendFaders_[channelIndex].emplace(
                            sendFaders_[channelIndex].begin() + sendPosition,
                            std::move(faderAndNode)
                        );
                        sendDestinations_[channelIndex].emplace(
                            sendDestinations_[channelIndex].begin() + sendPosition,
                            destination
                        );
                        if(batchUpdater_)
                        {
                            if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                            {
                                batchUpdater_->addObject(std::move(multiInputWithPDC));
                            }
                            batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                            oldSummingAndNode.first = std::move(newSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                        }
                        else
                        {
                            auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                            std::swap(newSummingAndNode.first, oldSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            connectionUpdatedCallback_(*this);
                        }
                        return {true};
                    }
                }
            }
            else if(destination.type == Position::Type::BusAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIdAndIndex_.begin(),
                    channelIdAndIndex_.end(),
                    destination.id
                );
                if(it != channelIdAndIndex_.end() && it->id == destination.id)
                {
                    auto type = channelInfo_[it->index].channelType;
                    if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                    {
                        auto& oldSummingAndNode = inputDevices_[it->index];
                        auto fromNode = isPreFader? mutes_[channelIndex].second: faders_[channelIndex].second;
                        if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                        {
                            auto newSummingAndNode = appendInputGroup(inputDevices_[it->index]);
                            graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                            graph_.connect(
                                newSummingAndNode.second, polarityInverters_[it->index].second, 0, 0
                            );
                            auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                                fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                            );
                            sendPolarityInverters_[channelIndex].emplace(
                                sendPolarityInverters_[channelIndex].begin() + sendPosition,
                                std::move(polarityInverterAndNode)
                            );
                            sendMutes_[channelIndex].emplace(
                                sendMutes_[channelIndex].begin() + sendPosition,
                                std::move(muteAndNode)
                            );
                            sendFaders_[channelIndex].emplace(
                                sendFaders_[channelIndex].begin() + sendPosition,
                                std::move(faderAndNode)
                            );
                            sendDestinations_[channelIndex].emplace(
                                sendDestinations_[channelIndex].begin() + sendPosition,
                                destination
                            );
                            if(batchUpdater_)
                            {
                                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                                {
                                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                                }
                                batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                                oldSummingAndNode.first = std::move(newSummingAndNode.first);
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            }
                            else
                            {
                                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                                std::unique_ptr<YADAW::Audio::Util::Summing> oldSumming(
                                    static_cast<YADAW::Audio::Util::Summing*>(oldSummingAndNode.first.release())
                                );
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                                connectionUpdatedCallback_(*this);
                            }
                            return {true};
                        }
                    }
                }
            }
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setChannelSendPreFader(
    std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader)
{
    if(channelIndex < channelCount())
    {
        const auto& sendPolarityInverters = sendPolarityInverters_[channelIndex];
        if(sendIndex < sendPolarityInverters_.size())
        {
            auto inNode = sendPolarityInverters[sendIndex].second->inNodes().front();
            auto prevIsPreFader = inNode == mutes_[channelIndex].second;
            if(prevIsPreFader != preFader)
            {
                auto newInNode = preFader? mutes_[channelIndex].second: faders_[channelIndex].second;
                graph_.disconnect(sendPolarityInverters[sendIndex].second->inEdges().front());
                graph_.connect(newInNode, sendPolarityInverters[channelIndex].second, 0, 0);
                return {true};
            }
            return {false};
        }
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setChannelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination)
{
    if(channelIndex < channelCount() && sendIndex < sendDestinations_[channelIndex].size())
    {
        auto oldDestination = sendDestinations_[channelIndex][sendIndex];
        ade::NodeHandle inNode;
        std::uint32_t prevChannelIndex = 0;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> oldSumming;
        ade::NodeHandle oldSummingNode;
        if(oldDestination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = audioOutputSummings_[it->index].second;
        }
        else if(oldDestination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = inputDevices_[it->index].second;
        }
        if(oldSummingNode != nullptr)
        {
            inNode = sendPolarityInverters_[channelIndex][sendIndex].second->inNodes().front();
            for(const auto& outEdge: inNode->outEdges())
            {
                if(outEdge->dstNode() == oldSummingNode)
                {
                    prevChannelIndex = graph_.getEdgeData(outEdge).toChannel;
                    break;
                }
            }
        }
        auto connected = false;
        if(destination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                destination.id
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
            {
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                {
                    auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                    graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                    oldSumming = std::move(oldSummingAndNode.first);
                    oldSummingNode = oldSummingAndNode.second;
                    oldSummingAndNode = std::move(newSummingAndNode);
                    connected = true;
                }
            }
        }
        else if(destination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                destination.id
            );
            if(it != channelIdAndIndex_.end() && it->id == destination.id)
            {
                auto type = channelInfo_[it->index].channelType;
                if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                {
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                        oldSumming.reset(oldSummingAndNode.first.release());
                        oldSummingNode = oldSummingAndNode.second;
                        oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                        oldSummingAndNode.second = std::move(newSummingAndNode.second);
                        connected = true;
                    }
                }
            }
        }
        else if(destination.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(connected)
        {
            if(batchUpdater_)
            {
                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingNode))
                {
                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                }
            }
            else
            {
                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingNode);
                connectionUpdatedCallback_(*this);
            }
        }
        else if(oldSummingNode != nullptr)
        {
            graph_.connect(inNode, oldSummingNode, 0, prevChannelIndex);
        }
        return {connected};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::removeChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount)
{
    if(channelIndex < channelCount())
    {
        auto& sendDestinations = sendDestinations_[channelIndex];
        auto& sendFaders = sendFaders_[channelIndex];
        auto& sendPolarityInverters = sendPolarityInverters_[channelIndex];
        auto& sendMutes = sendMutes_[channelIndex];
        auto sendCount = sendDestinations.size();
        if(sendPosition < sendCount && sendPosition + removeCount <= sendCount)
        {
            std::vector<Position> destinations;
            destinations.reserve(removeCount);
            FOR_RANGE(i, sendPosition, sendPosition + removeCount)
            {
                if(std::find(destinations.begin(), destinations.end(), sendDestinations[i]) != destinations.end())
                {
                    destinations.emplace_back(sendDestinations[i]);
                }
                graph_.removeNode(sendFaders[i].second);
                graph_.removeNode(sendMutes[i].second);
                graph_.removeNode(sendPolarityInverters[i].second);
            }
            std::vector<SummingAndNode> removingSummings;
            removingSummings.reserve(destinations.size());
            FOR_RANGE0(i, destinations.size())
            {
                auto destination = destinations[i];
                if(destination.type == Position::Type::AudioHardwareIOChannel)
                {
                    auto it = std::lower_bound(
                        audioOutputChannelIdAndIndex_.begin(),
                        audioOutputChannelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    removingSummings.emplace_back(
                        shrinkInputGroups(oldSummingAndNode)
                    ).swap(oldSummingAndNode);
                }
                else if(destination.type == Position::Type::PluginAuxIO)
                {
                    auto it = std::lower_bound(
                        channelIdAndIndex_.begin(),
                        channelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& newSummingAndNode = removingSummings.emplace_back(shrinkInputGroups(oldSummingAndNode));
                    std::swap(oldSummingAndNode.second, newSummingAndNode.second);
                    auto ptr = oldSummingAndNode.first.release();
                    oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                }
            }
            if(batchUpdater_)
            {
                FOR_RANGE(i, sendPosition, sendPosition + removeCount)
                {
                    batchUpdater_->addObject(std::move(sendFaders[i]).first);
                    batchUpdater_->addObject(std::move(sendMutes[i]).first);
                    batchUpdater_->addObject(std::move(sendPolarityInverters[i]).first);
                }
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            sendDestinations.erase(
                sendDestinations.begin() + sendPosition,
                sendDestinations.begin() + sendPosition + removeCount
            );
            sendFaders.erase(
                sendFaders.begin() + sendPosition,
                sendFaders.begin() + sendPosition + removeCount
            );
            sendMutes.erase(
                sendMutes.begin() + sendPosition,
                sendMutes.begin() + sendPosition + removeCount
            );
            sendPolarityInverters.erase(
                sendPolarityInverters.begin() + sendPosition,
                sendPolarityInverters.begin() + sendPosition + removeCount
            );
            return {true};
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::clearChannelSends(std::uint32_t channelIndex)
{
    if(channelIndex < channelCount())
    {
        return removeChannelSend(channelIndex, 0, sendDestinations_[channelIndex].size());
    }
    return std::nullopt;
}

std::optional<bool> Mixer::appendAudioOutputChannelSend(
    std::uint32_t channelIndex, bool isPreFader, Position destination)
{
    if(channelIndex < audioOutputChannelCount())
    {
        return insertAudioOutputChannelSend(channelIndex, audioOutputSendDestinations_[channelIndex].size(), isPreFader, destination);
    }
    return std::nullopt;
}

std::optional<bool> Mixer::insertAudioOutputChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination)
{
    if(channelIndex < audioOutputChannelCount())
    {
        if(sendPosition <= audioOutputSendPolarityInverters_[channelIndex].size())
        {
            if(destination.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    audioOutputChannelIdAndIndex_.begin(), audioOutputChannelIdAndIndex_.end(),
                    destination.id
                );
                if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    auto fromNode = isPreFader? audioOutputMutes_[channelIndex].second: audioOutputFaders_[channelIndex].second;
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                    {
                        auto newSummingAndNode = appendInputGroup(audioOutputSummings_[it->index]);
                        graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                        graph_.connect(
                            newSummingAndNode.second, audioOutputPolarityInverters_[it->index].second, 0, 0
                        );
                        auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                            fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                        );
                        audioOutputSendPolarityInverters_[channelIndex].emplace(
                            audioOutputSendPolarityInverters_[channelIndex].begin() + sendPosition,
                            std::move(polarityInverterAndNode)
                        );
                        audioOutputSendMutes_[channelIndex].emplace(
                            audioOutputSendMutes_[channelIndex].begin() + sendPosition,
                            std::move(muteAndNode)
                        );
                        audioOutputSendFaders_[channelIndex].emplace(
                            audioOutputSendFaders_[channelIndex].begin() + sendPosition,
                            std::move(faderAndNode)
                        );
                        audioOutputSendDestinations_[channelIndex].emplace(
                            audioOutputSendDestinations_[channelIndex].begin() + sendPosition,
                            destination
                        );
                        if(batchUpdater_)
                        {
                            if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                            {
                                batchUpdater_->addObject(std::move(multiInputWithPDC));
                            }
                            batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                            oldSummingAndNode.first = std::move(newSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                        }
                        else
                        {
                            auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                            std::swap(newSummingAndNode.first, oldSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            connectionUpdatedCallback_(*this);
                        }
                        return {true};
                    }
                }
            }
            else if(destination.type == Position::Type::BusAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                    destination.id
                );
                if(it != channelIdAndIndex_.end()
                    && it->id == destination.id)
                {
                    auto type = channelInfo_[it->index].channelType;
                    if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                    {
                        auto& oldSummingAndNode = inputDevices_[it->index];
                        auto fromNode = isPreFader? audioOutputMutes_[channelIndex].second: audioOutputFaders_[channelIndex].second;
                        if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                        {
                            auto newSummingAndNode = appendInputGroup(inputDevices_[it->index]);
                            graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                            graph_.connect(
                                newSummingAndNode.second, polarityInverters_[it->index].second, 0, 0
                            );
                            auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                                fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                            );
                            audioOutputSendPolarityInverters_[channelIndex].emplace(
                                audioOutputSendPolarityInverters_[channelIndex].begin() + sendPosition,
                                std::move(polarityInverterAndNode)
                            );
                            audioOutputSendMutes_[channelIndex].emplace(
                                audioOutputSendMutes_[channelIndex].begin() + sendPosition,
                                std::move(muteAndNode)
                            );
                            audioOutputSendFaders_[channelIndex].emplace(
                                audioOutputSendFaders_[channelIndex].begin() + sendPosition,
                                std::move(faderAndNode)
                            );
                            audioOutputSendDestinations_[channelIndex].emplace(
                                audioOutputSendDestinations_[channelIndex].begin() + sendPosition,
                                destination
                            );
                            if(batchUpdater_)
                            {
                                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                                {
                                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                                }
                                batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                                oldSummingAndNode.first = std::move(newSummingAndNode.first);
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            }
                            else
                            {
                                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                                std::unique_ptr<YADAW::Audio::Util::Summing> oldSumming(
                                    static_cast<YADAW::Audio::Util::Summing*>(oldSummingAndNode.first.release())
                                );
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                                connectionUpdatedCallback_(*this);
                            }
                            return {true};
                        }
                    }
                }
            }
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setAudioOutputChannelSendPreFader(
    std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader)
{
    if(channelIndex < audioOutputChannelCount())
    {
        const auto& audioOutputSendPolarityInverters = audioOutputSendPolarityInverters_[channelIndex];
        if(sendIndex < audioOutputSendPolarityInverters.size())
        {
            auto inNode = audioOutputSendPolarityInverters[sendIndex].second->inNodes().front();
            auto prevIsPreFader = inNode == audioOutputMutes_[channelIndex].second;
            if(prevIsPreFader != preFader)
            {
                auto newInNode = preFader? audioOutputMutes_[channelIndex].second: audioOutputFaders_[channelIndex].second;
                graph_.disconnect(audioOutputSendPolarityInverters[sendIndex].second->inEdges().front());
                graph_.connect(newInNode, audioOutputSendPolarityInverters[channelIndex].second, 0, 0);
                return {true};
            }
            return {false};
        }
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setAudioOutputChannelSendDestination(
    std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination)
{
    if(channelIndex < audioOutputChannelCount() && sendIndex < audioOutputSendDestinations_[channelIndex].size())
    {
        auto oldDestination = audioOutputSendDestinations_[channelIndex][sendIndex];
        ade::NodeHandle inNode;
        std::uint32_t prevChannelIndex = 0;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> oldSumming;
        ade::NodeHandle oldSummingNode;
        if(oldDestination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = audioOutputSummings_[it->index].second;
        }
        else if(oldDestination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldDestination.id
            );
            oldSummingNode = inputDevices_[it->index].second;
        }
        else if(oldDestination.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(oldSummingNode != nullptr)
        {
            inNode = audioOutputSendPolarityInverters_[channelIndex][sendIndex].second->inNodes().front();
            for(const auto& outEdge: inNode->outEdges())
            {
                if(outEdge->dstNode() == oldSummingNode)
                {
                    prevChannelIndex = graph_.getEdgeData(outEdge).toChannel;
                    break;
                }
            }
        }
        auto connected = false;
        if(destination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                destination.id
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == destination.id)
            {
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                {
                    auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                    graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                    oldSumming = std::move(oldSummingAndNode.first);
                    oldSummingNode = oldSummingAndNode.second;
                    oldSummingAndNode = std::move(newSummingAndNode);
                    connected = true;
                }
            }
        }
        else if(destination.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                destination.id
            );
            if(it != channelIdAndIndex_.end() && it->id == destination.id)
            {
                auto type = channelInfo_[it->index].channelType;
                if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                {
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                        oldSumming.reset(oldSummingAndNode.first.release());
                        oldSummingNode = oldSummingAndNode.second;
                        oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                        oldSummingAndNode.second = std::move(newSummingAndNode.second);
                        connected = true;
                    }
                }
            }
        }
        else if(destination.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(connected)
        {
            if(batchUpdater_)
            {
                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingNode))
                {
                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                }
            }
            else
            {
                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingNode);
                connectionUpdatedCallback_(*this);
            }
        }
        else if(oldSummingNode != nullptr)
        {
            graph_.connect(inNode, oldSummingNode, 0, prevChannelIndex);
        }
        return {connected};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::removeAudioOutputChannelSend(
    std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount)
{
    if(channelIndex < audioOutputChannelCount())
    {
        auto& sendDestinations = audioOutputSendDestinations_[channelIndex];
        auto& sendFaders = audioOutputSendFaders_[channelIndex];
        auto& sendPolarityInverters = audioOutputSendPolarityInverters_[channelIndex];
        auto& sendMutes = audioOutputSendMutes_[channelIndex];
        auto sendCount = sendDestinations.size();
        if(sendPosition < sendCount && sendPosition + removeCount <= sendCount)
        {
            std::vector<Position> destinations;
            destinations.reserve(removeCount);
            FOR_RANGE(i, sendPosition, sendPosition + removeCount)
            {
                if(std::find(destinations.begin(), destinations.end(), sendDestinations[i]) != destinations.end())
                {
                    destinations.emplace_back(sendDestinations[i]);
                }
                graph_.removeNode(sendFaders[i].second);
                graph_.removeNode(sendMutes[i].second);
                graph_.removeNode(sendPolarityInverters[i].second);
            }
            std::vector<SummingAndNode> removingSummings;
            removingSummings.reserve(destinations.size());
            FOR_RANGE0(i, destinations.size())
            {
                auto destination = destinations[i];
                if(destination.type == Position::Type::AudioHardwareIOChannel)
                {
                    auto it = std::lower_bound(
                        audioOutputChannelIdAndIndex_.begin(),
                        audioOutputChannelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    removingSummings.emplace_back(
                        shrinkInputGroups(oldSummingAndNode)
                    ).swap(oldSummingAndNode);
                }
                else if(destination.type == Position::Type::PluginAuxIO)
                {
                    auto it = std::lower_bound(
                        channelIdAndIndex_.begin(),
                        channelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& newSummingAndNode = removingSummings.emplace_back(shrinkInputGroups(oldSummingAndNode));
                    std::swap(oldSummingAndNode.second, newSummingAndNode.second);
                    auto ptr = oldSummingAndNode.first.release();
                    oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                }
            }
            if(batchUpdater_)
            {
                FOR_RANGE(i, sendPosition, sendPosition + removeCount)
                {
                    batchUpdater_->addObject(std::move(sendFaders[i].first));
                    batchUpdater_->addObject(std::move(sendMutes[i].first));
                    batchUpdater_->addObject(std::move(sendPolarityInverters[i].first));
                }
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            sendDestinations.erase(
                sendDestinations.begin() + sendPosition,
                sendDestinations.begin() + sendPosition + removeCount
            );
            sendFaders.erase(
                sendFaders.begin() + sendPosition,
                sendFaders.begin() + sendPosition + removeCount
            );
            sendMutes.erase(
                sendMutes.begin() + sendPosition,
                sendMutes.begin() + sendPosition + removeCount
            );
            sendPolarityInverters.erase(
                sendPolarityInverters.begin() + sendPosition,
                sendPolarityInverters.begin() + sendPosition + removeCount
            );
            return {true};
        }
        return {false};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::clearAudioOutputChannelSends(std::uint32_t channelIndex)
{
    if(channelIndex < audioOutputChannelCount())
    {
        return removeAudioOutputChannelSend(channelIndex, 0, audioOutputSendDestinations_[channelIndex].size());
    }
    return std::nullopt;
}

ade::NodeHandle Mixer::getInstrument(std::uint32_t index) const
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Instrument)
    {
        return inputDevices_[index].second;
    }
    return {};
}

OptionalRef<const Context> Mixer::getInstrumentContext(std::uint32_t index) const
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Instrument
        && instrumentContexts_[index])
    {
        return instrumentContexts_[index];
    }
    return {};
}

OptionalRef<Context> Mixer::getInstrumentContext(std::uint32_t index)
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Instrument
        && instrumentContexts_[index])
    {
        return instrumentContexts_[index];
    }
    return {};
}

bool Mixer::setInstrument(
    std::uint32_t index, ade::NodeHandle nodeHandle,
    std::uint32_t outputChannelIndex)
{
    auto ret = false;
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Instrument
        && nodeHandle != nullptr
        && inputDevices_[index].second == nullptr)
    {
        auto device = graph_.getNodeData(nodeHandle).process.device();
        auto polarityInverterNode = polarityInverters_[index].second;
        if(outputChannelIndex < device->audioOutputGroupCount())
        {
            ret = graph_.connect(nodeHandle, polarityInverterNode, outputChannelIndex, 0).has_value();
            if(ret)
            {
                inputDevices_[index].second = nodeHandle;
                instrumentBypassed_[index] = false;
                instrumentOutputChannelIndex_[index] = outputChannelIndex;
                auto& auxInputIDs  = instrumentAuxInputIDs_ [index];
                auto& auxOutputIDs = instrumentAuxOutputIDs_[index];
                auxInputIDs.clear();
                auxInputIDs.reserve(
                    device->audioInputGroupCount()
                );
                std::generate_n(
                    std::back_inserter(auxInputIDs),
                    device->audioInputGroupCount(),
                    [this]()
                    {
                        return auxInputIdGen_();
                    }
                );
                auxOutputIDs.clear();
                auxOutputIDs.reserve(
                    device->audioOutputGroupCount()
                );
                std::generate_n(
                    std::back_inserter(auxOutputIDs),
                    device->audioOutputGroupCount() - 1,
                    [this]()
                    {
                        return auxOutputIdGen_();
                    }
                );
            }
        }
    }
    return ret;
}

bool Mixer::setInstrument(
    std::uint32_t index, ade::NodeHandle nodeHandle, Context&& context,
    std::uint32_t outputChannelIndex)
{
    auto ret = setInstrument(index, nodeHandle, outputChannelIndex);
    if(ret)
    {
        instrumentContexts_[index] = std::move(context);
    }
    return ret;
}

std::pair<ade::NodeHandle, Context> Mixer::detachInstrument(std::uint32_t index)
{
    if(index < channelCount()
        && inputDevices_[index].second != nullptr
        && channelInfo_[index].channelType == ChannelType::Instrument)
    {
        auto instrumentNode = inputDevices_[index].second;
        auto polarityInverterNode = polarityInverters_[index].second;
        graph_.disconnect(polarityInverterNode->inEdges().front());
        inputDevices_[index].second = nullptr;
        return std::make_pair(
            std::move(instrumentNode),
            std::move(instrumentContexts_[index])
        );
    }
    return std::make_pair(ade::NodeHandle(), YADAW::Util::createUniquePtr(nullptr));
}

std::optional<IDGen::ID> Mixer::instrumentAuxInputID(
    std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const
{
    if(channelIndex < channelCount()
        && channelGroupIndex < instrumentAuxInputIDs_[channelIndex].size())
    {
        return {instrumentAuxInputIDs_[channelIndex][channelGroupIndex]};
    }
    return std::nullopt;
}

std::optional<IDGen::ID> Mixer::instrumentAuxOutputID(
    std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const
{
    if(channelIndex < channelCount()
        && channelGroupIndex <= instrumentAuxOutputIDs_[channelIndex].size())
    {
        if(channelGroupIndex > preFaderInserts_[channelIndex]->inChannelGroupIndex())
        {
            return {instrumentAuxInputIDs_[channelIndex][channelGroupIndex - 1]};
        }
        if(channelGroupIndex <= preFaderInserts_[channelIndex]->inChannelGroupIndex())
        {
            return {instrumentAuxInputIDs_[channelIndex][channelGroupIndex]};
        }
    }
    return std::nullopt;
}

bool Mixer::isInstrumentBypassed(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return instrumentBypassed_[index];
    }
    return false;
}

void Mixer::setInstrumentBypass(std::uint32_t index, bool bypass)
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Instrument
        && inputDevices_[index].second != nullptr)
    {
        if(instrumentBypassed_[index] != bypass)
        {
            if(bypass)
            {
                for(const auto& edge: inputDevices_[index].second->outEdges())
                {
                    if(edge->dstNode() == polarityInverters_[index].second)
                    {
                        graph_.disconnect(edge);
                        break;
                    }
                }
            }
            else
            {
                graph_.connect(
                    inputDevices_[index].second, polarityInverters_[index].second,
                    instrumentOutputChannelIndex_[index], 0
                );
            }
            instrumentBypassed_[index] = bypass;
        }
    }
}

void Mixer::setConnectionUpdatedCallback(
    ConnectionUpdatedCallback* callback)
{
    connectionUpdatedCallback_ = callback;
}

void Mixer::resetConnectionUpdatedCallback()
{
    connectionUpdatedCallback_ = &Impl::blankConnectionUpdatedCallback;
}

void Mixer::connectionUpdated() const
{
    connectionUpdatedCallback_(*this);
}

void Mixer::setPreInsertAudioInputChannelCallback(
    std::function<PreInsertChannelCallback>&& callback)
{
    preInsertAudioInputChannelCallback_ = std::move(callback);
}

void Mixer::setPreInsertRegularChannelCallback(std::function<PreInsertChannelCallback>&& callback)
{
    preInsertRegularChannelCallback_ = std::move(callback);
}

void Mixer::setPreInsertAudioOutputChannelCallback(std::function<PreInsertChannelCallback>&& callback)
{
    preInsertAudioOutputChannelCallback_ = std::move(callback);
}

void Mixer::resetPreInsertAudioInputChannelCallback()
{
    preInsertAudioInputChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

void Mixer::resetPreInsertRegularChannelCallback()
{
    preInsertRegularChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

void Mixer::resetPreInsertAudioOutputChannelCallback()
{
    preInsertAudioOutputChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

OptionalRef<YADAW::Util::BatchUpdater> Mixer::batchUpdater()
{
    if(batchUpdater_)
    {
        return {*batchUpdater_};
    }
    return std::nullopt;
}

void Mixer::setBatchUpdater(YADAW::Util::BatchUpdater& batchUpdater)
{
    batchUpdater_ = &batchUpdater;
    auto vecs = {
        std::ranges::ref_view(audioInputPreFaderInserts_),
        std::ranges::ref_view(audioInputPostFaderInserts_),
        std::ranges::ref_view(preFaderInserts_),
        std::ranges::ref_view(postFaderInserts_),
        std::ranges::ref_view(audioOutputPreFaderInserts_),
        std::ranges::ref_view(audioOutputPostFaderInserts_)
    };
    auto view = std::ranges::views::join(vecs);
    for(auto& inserts: view)
    {
        inserts->setBatchUpdater(*batchUpdater_);
    }
}

void Mixer::resetBatchUpdater()
{
    batchUpdater_ = nullptr;
    auto vecs = {
        std::ranges::ref_view(audioInputPreFaderInserts_),
        std::ranges::ref_view(audioInputPostFaderInserts_),
        std::ranges::ref_view(preFaderInserts_),
        std::ranges::ref_view(postFaderInserts_),
        std::ranges::ref_view(audioOutputPreFaderInserts_),
        std::ranges::ref_view(audioOutputPostFaderInserts_)
    };
    auto view = std::ranges::views::join(vecs);
    for(auto& inserts: view)
    {
        inserts->resetBatchUpdater();
    }
}

std::optional<ade::EdgeHandle> Mixer::addConnection(
    const ade::NodeHandle& from, const ade::NodeHandle& to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto ret = graph_.connect(from, to, fromChannel, toChannel);
    if(ret.has_value())
    {
        connections_.emplace(*ret);
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
    }
    return ret;
}

void Mixer::removeConnection(const ade::EdgeHandle& edgeHandle)
{
    if(auto it = connections_.find(edgeHandle); it != connections_.end())
    {
        graph_.disconnect(*it);
        connections_.erase(it);
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
    }
}

void Mixer::clearConnections()
{
    for(const auto& connection: connections_)
    {
        graph_.disconnect(connection);
    }
    if(batchUpdater_)
    {
        batchUpdater_->addNull();
    }
    else
    {
        connectionUpdatedCallback_(*this);
    }
    connections_.clear();
}

std::tuple<Mixer::PolarityInverterAndNode, Mixer::MuteAndNode, Mixer::FaderAndNode> Mixer::createSend(
    ade::NodeHandle fromNode, ade::NodeHandle toNode,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto& channelGroup = graph_.getNodeData(fromNode).process.device()->audioOutputGroupAt(fromChannel)->get();
    auto channelGroupType = channelGroup.type();
    auto channelCountInGroup = channelGroup.channelCount();
    auto polarityInverter = std::make_unique<PolarityInverter>(channelGroupType, channelCountInGroup);
    auto polarityInverterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter));
    auto mute = std::make_unique<YADAW::Audio::Util::Mute>(channelGroupType, channelCountInGroup);
    auto muteNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*mute));
    auto fader = volumeFaderFactory_(channelGroupType, channelCountInGroup);
    auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
    graph_.connect(fromNode, polarityInverterNode, fromChannel, 0);
    graph_.connect(polarityInverterNode, muteNode, 0, 0);
    graph_.connect(muteNode, faderNode, 0, 0);
    graph_.connect(faderNode, toNode, 0, toChannel);
    return {
        std::pair{std::move(polarityInverter), std::move(polarityInverterNode)},
        std::pair{std::move(mute), std::move(muteNode)},
        std::pair{std::move(fader), std::move(faderNode)}
    };
}
}