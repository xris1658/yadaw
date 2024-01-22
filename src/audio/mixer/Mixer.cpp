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
        return {std::ref(audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}
}