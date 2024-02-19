#include "Mixer.hpp"

#include "audio/mixer/BlankGenerator.hpp"
#include "audio/mixer/BlankReceiver.hpp"

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

const YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
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
                graph_, inNode, faderNode, channel, 0
            )
        );
        audioInputPostFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        audioInputFaders_.emplace_back(std::move(fader), faderNode);
        audioInputMeters_.emplace_back(std::move(meter), meterNode);
        auto& info = audioInputChannelInfo_.emplace_back();
        info.channelType = ChannelType::AudioBus;
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
            audioInputPreFaderInserts_.begin() + last
        );
        audioInputPostFaderInserts_.erase(
            audioInputPostFaderInserts_.begin() + first,
            audioInputPostFaderInserts_.begin() + last
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
            0, channelGroup.type(), channelGroup.channelCount()
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
                graph_, summingNode, faderNode, 0, 0
            )
        );
        audioOutputPostFaderInserts_.emplace_back(
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        graph_.connect(meterNode, outNode, 0, channel);
        audioOutputSummings_.emplace_back(std::move(summing), summingNode);
        audioOutputFaders_.emplace_back(std::move(fader), faderNode);
        audioOutputMeters_.emplace_back(std::move(meter), meterNode);
        auto& info = audioOutputChannelInfo_.emplace_back();
        info.channelType = ChannelType::AudioBus;
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
        audioOutputMeters_.erase(
            audioOutputMeters_.begin() + first,
            audioOutputMeters_.begin() + last
        );
        audioOutputFaders_.erase(
            audioOutputFaders_.begin() + first,
            audioOutputFaders_.begin() + last
        );
        audioOutputChannelInfo_.erase(
            audioOutputChannelInfo_.begin() + first,
            audioOutputChannelInfo_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioOutputChannels()
{
    removeAudioOutputChannel(0, audioOutputChannelCount());
}

bool Mixer::insertChannel(
    std::uint32_t position, Mixer::ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    if(position <= channelCount())
    {
        switch(channelType)
        {
        case ChannelType::Audio:
        case ChannelType::Instrument:
        {
            auto blankGenerator = new (std::nothrow) YADAW::Audio::Mixer::BlankGenerator(
                channelGroupType, channelCountInGroup
            );
            if(!blankGenerator)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> inputDevice(blankGenerator);
            auto blankReceiver = new (std::nothrow) YADAW::Audio::Mixer::BlankReceiver(
                channelGroupType, channelCountInGroup
            );
            if(!blankReceiver)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> outputDevice(blankReceiver);
            auto inputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankGenerator));
            auto outputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankReceiver));
            auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
                channelGroupType, channelCountInGroup
            );
            auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
            auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
                8192, channelGroupType, channelCountInGroup
            );
            auto meterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*meter));
            auto preFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inputDeviceNode, faderNode, 0, 0
            );
            auto postFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            );
            inputDevices_.emplace(inputDevices_.begin() + position,
                std::move(inputDevice), inputDeviceNode
            );
            preFaderInserts_.emplace(preFaderInserts_.begin() + position,
                std::move(preFaderInserts)
            );
            faders_.emplace(faders_.begin() + position,
                std::move(fader), faderNode
            );
            postFaderInserts_.emplace(postFaderInserts_.begin() + position,
                std::move(postFaderInserts)
            );
            meters_.emplace(meters_.begin() + position,
                std::move(meter), meterNode
            );
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            auto it = channelInfo_.emplace(channelInfo_.begin() + position);
            it->channelType = channelType;
            return true;
        }
        case ChannelType::AudioFX:
        case ChannelType::AudioBus:
        {
            auto summing = new (std::nothrow) YADAW::Audio::Util::Summing(
                1, channelGroupType, channelCountInGroup
            );
            if(!summing)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> inputDevice(summing);
            auto blankReceiver = new (std::nothrow) YADAW::Audio::Mixer::BlankReceiver(
                channelGroupType, channelCountInGroup
            );
            if(!blankReceiver)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> outputDevice(blankReceiver);
            auto inputDeviceNode = graphWithPDC_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*summing));
            auto outputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankReceiver));
            auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
                channelGroupType, channelCountInGroup
            );
            auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
            auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
                8192, channelGroupType, channelCountInGroup
            );
            auto meterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*meter));
            auto preFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inputDeviceNode, faderNode, 0, 0
            );
            auto postFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            );
            inputDevices_.emplace(inputDevices_.begin() + position,
                std::move(inputDevice), inputDeviceNode
            );
            preFaderInserts_.emplace(preFaderInserts_.begin() + position,
                std::move(preFaderInserts)
            );
            faders_.emplace(faders_.begin() + position,
                std::move(fader), faderNode
            );
            postFaderInserts_.emplace(postFaderInserts_.begin() + position,
                std::move(postFaderInserts)
            );
            meters_.emplace(meters_.begin() + position,
                std::move(meter), meterNode
            );
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            auto it = channelInfo_.emplace(channelInfo_.begin() + position);
            it->channelType = channelType;
            return true;
        }
        }
    }
    return false;
}

bool Mixer::appendChannel(
    Mixer::ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    return insertChannel(
        channelCount(), channelType, channelGroupType, channelCountInGroup);
}

bool Mixer::removeChannel(std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < channelCount() && last <= channelCount())
    {
        preFaderInserts_.erase(
            preFaderInserts_.begin() + first,
            preFaderInserts_.begin() + last
        );
        postFaderInserts_.erase(
            postFaderInserts_.begin() + first,
            postFaderInserts_.begin() + last
        );
        std::for_each_n(inputDevices_.begin() + first, removeCount,
            [this](auto& deviceAndNode)
            {
                auto& [device, nodeHandle] = deviceAndNode;
                graph_.removeNode(nodeHandle);
            }
        );
        std::for_each_n(faders_.begin() + first, removeCount,
            [this](auto& deviceAndNode)
            {
                auto& [device, nodeHandle] = deviceAndNode;
                graph_.removeNode(nodeHandle);
            }
        );
        std::for_each_n(meters_.begin() + first, removeCount,
            [this](auto& deviceAndNode)
            {
                auto& [device, nodeHandle] = deviceAndNode;
                graph_.removeNode(nodeHandle);
            }
        );
        std::for_each_n(outputDevices_.begin() + first, removeCount,
            [this](auto& deviceAndNode)
            {
                auto& [device, nodeHandle] = deviceAndNode;
                graph_.removeNode(nodeHandle);
            }
        );
        inputDevices_.erase(
            inputDevices_.begin() + first,
            inputDevices_.begin() + last
        );
        meters_.erase(
            meters_.begin() + first,
            meters_.begin() + last
        );
        faders_.erase(
            faders_.begin() + first,
            faders_.begin() + last
        );
        outputDevices_.erase(
            outputDevices_.begin() + first,
            outputDevices_.begin() + last
        );
        channelInfo_.erase(
            channelInfo_.begin() + first,
            channelInfo_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearChannels()
{
    removeChannel(0, channelCount());
}
}