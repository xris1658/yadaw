#include "Mixer.hpp"

namespace YADAW::Audio::Mixer
{
Mixer::Mixer():
    graph_(),
    graphWithPDC_(graph_)
{}

const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph() const
{
    return graphWithPDC_;
}

YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph()
{
    return graphWithPDC_;
}

const YADAW::Audio::Engine::Extension::Buffer& Mixer::buffer() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

YADAW::Audio::Engine::Extension::Buffer& Mixer::buffer()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

std::uint32_t Mixer::audioInputChannelCount() const
{
    return audioInputFaders_.size();
}

std::uint32_t Mixer::channelCount() const
{
    return audioOutputFaders_.size();
}

std::uint32_t Mixer::audioOutputChannelCount() const
{
    return faders_.size();
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

bool Mixer::appendAudioInputChannel(
    const ade::NodeHandle& inNode, std::uint32_t channel)
{
    auto device = graph_.getNodeData(inNode).process.device();
    if(channel < device->audioOutputGroupCount())
    {
        const auto& channelGroup = device->audioOutputGroupAt(channel)->get();
        auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
            8192, channelGroup.type(), channelGroup.channelCount()
        );
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        audioInputPreFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inNode, meterNode, channel, 0
            )
        );
        audioInputPostFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, meterNode, faderNode, 0, 0
            )
        );
        audioInputFaders_.emplace_back(std::move(fader), faderNode);
        audioInputMeters_.emplace_back(std::move(meter), meterNode);
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
        audioInputPreFaderInserts_.erase(
            audioInputPreFaderInserts_.begin() + first,
            audioInputPreFaderInserts_.begin() + first + removeCount
        );
        audioInputPostFaderInserts_.erase(
            audioInputPostFaderInserts_.begin() + first,
            audioInputPostFaderInserts_.begin() + first + removeCount
        );
        audioInputFaders_.erase(
            audioInputFaders_.begin() + first,
            audioInputFaders_.begin() + first + removeCount
        );
        audioInputMeters_.erase(
            audioInputMeters_.begin() + first,
            audioInputMeters_.begin() + first + removeCount
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
    const ade::NodeHandle& outNode, std::uint32_t channel)
{
    auto device = graph_.getNodeData(outNode).process.device();
    if(channel < device->audioInputGroupCount())
    {
        const auto& channelGroup = device->audioInputGroupAt(channel)->get();
        auto summing = std::make_unique<YADAW::Audio::Util::Summing>(
            1, channelGroup.type(), channelGroup.channelCount()
        );
        auto summingNode = graphWithPDC_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*summing)
        );
        auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
            8192, channelGroup.type(), channelGroup.channelCount()
        );
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        audioOutputPreFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, summingNode, meterNode, 0, 0
            )
        );
        audioOutputPostFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, meterNode, faderNode, 0, 0
            )
        );
        graph_.connect(meterNode, outNode, 0, channel);
        audioOutputSummings_.emplace_back(std::move(summing), summingNode);
        audioOutputFaders_.emplace_back(std::move(fader), faderNode);
        audioOutputMeters_.emplace_back(std::move(meter), meterNode);
        return true;
    }
    return false;
}

bool Mixer::removeAudioOutputChannel(std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < audioOutputChannelCount()
            && last <= audioOutputChannelCount()
    )
    {
        audioOutputMeters_.erase(
            audioOutputMeters_.begin() + first,
            audioOutputMeters_.begin() + last
        );
        audioOutputPreFaderInserts_.erase(
            audioOutputPreFaderInserts_.begin() + first,
            audioOutputPreFaderInserts_.begin() + last
        );
        audioOutputPostFaderInserts_.erase(
            audioOutputPostFaderInserts_.begin() + first,
            audioOutputPostFaderInserts_.begin() + last
        );
        audioOutputSummings_.erase(
            audioOutputSummings_.begin() + first,
            audioOutputSummings_.begin() + last
        );
        audioOutputFaders_.erase(
            audioOutputFaders_.begin() + first,
            audioOutputFaders_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioOutputChannels()
{
    removeAudioOutputChannel(0, audioOutputChannelCount());
}

bool Mixer::insertChannel(std::uint32_t position, Mixer::ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType chanelGroupType, std::uint32_t channelCount)
{
    return false;
}

bool Mixer::appendChannel(Mixer::ChannelType channelType, YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    return false;
}

void Mixer::removeChannel(std::uint32_t position, std::uint32_t removeCount)
{

}

void Mixer::clearChannels()
{

}
}